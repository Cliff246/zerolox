#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"
#include <algorithm>
#include <cstdint>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <vector>
using namespace llvm;
namespace {
struct Emit {
  raw_ostream &out;
  explicit Emit(raw_ostream &out):out(out){}
  std::map<const Value*,std::string> regs;
  std::map<const BasicBlock*,std::string> labels;
  std::set<std::string> occupied;
  std::map<const Instruction*,std::vector<std::string>> releaseAfter;
  std::string temp(){
    for(unsigned n=0;n<6;++n){
      auto name="t"+std::to_string(n);
      if(occupied.insert(name).second)return name;
    }
    report_fatal_error("ZLX prototype needs a spill");
  }
  SmallVector<std::string, 2> scratch;
  std::map<const GetElementPtrInst *, int32_t> foldedOffsets;

  static bool freeZeroExtend(const Instruction &instruction) {
    const auto *extend = dyn_cast<ZExtInst>(&instruction);
    return extend && extend->getDestTy()->getIntegerBitWidth() <= 32 &&
           isa<LoadInst, ICmpInst>(extend->getOperand(0));
  }

  static std::optional<int32_t> constantOffset(const GetElementPtrInst &gep) {
    if (gep.getPointerAddressSpace() != 0)
      return std::nullopt;
    const auto &layout = gep.getModule()->getDataLayout();
    APInt offset(layout.getIndexSizeInBits(0), 0);
    if (!gep.accumulateConstantOffset(layout, offset))
      return std::nullopt;
    return offset.getSExtValue();
  }

  void findFoldedOffsets(Function &function) {
    foldedOffsets.clear();
    for (auto &block : function) {
      for (auto &instruction : block) {
        auto *gep = dyn_cast<GetElementPtrInst>(&instruction);
        // Arguments stay live independently of where a folded access uses them.
        if (!gep || !isa<Argument>(gep->getPointerOperand()) || gep->use_empty())
          continue;
        auto offset = constantOffset(*gep);
        if (!offset || *offset < -256 || *offset > 255)
          continue;
        if (all_of(gep->uses(), [](const Use &use) {
              return (isa<LoadInst>(use.getUser()) && use.getOperandNo() == 0) ||
                     (isa<StoreInst>(use.getUser()) && use.getOperandNo() == 1);
            }))
          foldedOffsets.emplace(gep, *offset);
      }
    }
  }

  static const Argument *foldedZeroTest(const ICmpInst &compare) {
    if (!compare.isEquality() || !compare.hasOneUse() || !compare.getOperand(0)->getType()->isIntegerTy(32))
      return nullptr;
    const auto *select = dyn_cast<SelectInst>(*compare.user_begin());
    if (!select || select->getCondition() != &compare)
      return nullptr;
    for (unsigned argument = 0; argument < 2; ++argument) {
      const auto *constant = dyn_cast<ConstantInt>(compare.getOperand(1 - argument));
      if (constant && constant->isZero())
        return dyn_cast<Argument>(compare.getOperand(argument));
    }
    return nullptr;
  }

  bool emitsCode(const Instruction &instruction) const {
    if (const auto *compare = dyn_cast<ICmpInst>(&instruction); compare && foldedZeroTest(*compare))
      return false;
    return !freeZeroExtend(instruction) &&
           !foldedOffsets.count(dyn_cast<GetElementPtrInst>(&instruction));
  }

  SmallVector<const Instruction *, 4> effectiveUsers(const Instruction &instruction) {
    SmallVector<const Instruction *, 4> users;
    for (auto *user : instruction.users()) {
      auto *use = cast<Instruction>(user);
      if (freeZeroExtend(*use)) {
        for (auto *extendedUser : use->users())
          users.push_back(cast<Instruction>(extendedUser));
      } else {
        users.push_back(use);
      }
    }
    return users;
  }

  std::string materialize(uint32_t value) {
    if (value == 0)
      return "z0";
    auto name = temp();
    scratch.push_back(name);
    out << "get " << name << ", " << value << "\n";
    return name;
  }

  std::string reg(const Value *value) {
    if (auto *instruction = dyn_cast<Instruction>(value); instruction && freeZeroExtend(*instruction))
      value = instruction->getOperand(0);
    if (auto *integer = dyn_cast<ConstantInt>(value)) {
      if (integer->getBitWidth() > 32)
        report_fatal_error("ZLX expects integer constants of at most 32 bits");
      return materialize(integer->getZExtValue());
    }
    if (isa<ConstantPointerNull>(value))
      return "z0";
    auto mapping = regs.find(value);
    if (mapping == regs.end())
      report_fatal_error("ZLX value has no register");
    return mapping->second;
  }

  std::string address(Value *pointer) {
    auto entry = foldedOffsets.find(dyn_cast<GetElementPtrInst>(pointer));
    if (entry != foldedOffsets.end())
      return "[" + reg(entry->first->getPointerOperand()) + ", " + std::to_string(entry->second) + "]";
    return "[" + reg(pointer) + "]";
  }

  void move(StringRef dest, StringRef source) {
    if (dest != source)
      out << "mv " << dest << ", " << source << "\n";
  }

  void addOffset(StringRef dest, StringRef base, int32_t offset) {
    if (offset == 0) {
      move(dest, base);
    } else if (dest == base && offset >= -32 && offset <= 32) {
      out << "addi " << dest << ", " << offset << "\n";
    } else if (offset >= -1024 && offset <= 1023) {
      out << "addi " << dest << ", " << base << ", " << offset << "\n";
    } else {
      auto value = materialize(static_cast<uint32_t>(offset));
      out << "add " << dest << ", " << base << ", " << value << "\n";
    }
  }

  void shiftImmediate(StringRef op, StringRef dest, StringRef source, unsigned count) {
    if (count == 0) {
      move(dest, source);
      return;
    }
    out << op << " " << dest << ", ";
    if (dest != source)
      out << source << ", ";
    out << count << "\n";
  }

  void mask(StringRef dest, StringRef source, unsigned bits) {
    if (bits <= 10) {
      out << "andi " << dest << ", " << source << ", " << ((1u << bits) - 1) << "\n";
    } else {
      shiftImmediate("slli", dest, source, 32 - bits);
      shiftImmediate("srli", dest, dest, 32 - bits);
    }
  }

  void binary(BinaryOperator &instruction, StringRef dest) {
    if (!instruction.getType()->isIntegerTy(32))
      report_fatal_error("expected i32 ALU");
    auto left = reg(instruction.getOperand(0));
    StringRef op = instruction.getOpcodeName();
    bool logic = op == "and" || op == "or" || op == "xor";
    bool shift = op == "shl" || op == "lshr" || op == "ashr";
    if (auto *constant = dyn_cast<ConstantInt>(instruction.getOperand(1))) {
      auto immediate = constant->getSExtValue();
      if (op == "add") {
        addOffset(dest, left, immediate);
        return;
      }
      if (logic && immediate >= -1024 && immediate <= 1023) {
        out << op << "i " << dest << ", " << left << ", " << immediate << "\n";
        return;
      }
      if (shift && constant->getZExtValue() < 32) {
        shiftImmediate(op == "shl" ? "slli" : op == "lshr" ? "srli" : "srai",
                       dest, left, constant->getZExtValue());
        return;
      }
    }
    if (op != "add" && op != "sub" && !logic && !shift)
      report_fatal_error("unsupported ZLX ALU");
    auto right = reg(instruction.getOperand(1));
    if (logic && dest == "ac" && (left == "ac" || right == "ac")) {
      out << op << "a " << (left == "ac" ? right : left) << "\n";
      return;
    }
    if (shift)
      op = op == "shl" ? "sll" : op == "lshr" ? "srl" : "sra";
    out << op << " " << dest << ", " << left << ", " << right << "\n";
  }

  void compare(ICmpInst &instruction, StringRef dest) {
    auto *type = instruction.getOperand(0)->getType();
    if (!type->isIntegerTy(32) && !type->isPointerTy())
      report_fatal_error("ZLX comparison requires i32 or pointers");
    auto left = reg(instruction.getOperand(0));
    auto right = reg(instruction.getOperand(1));
    auto predicate = instruction.getPredicate();
    switch (predicate) {
      case CmpInst::ICMP_SGT: case CmpInst::ICMP_SLE:
      case CmpInst::ICMP_UGT: case CmpInst::ICMP_ULE:
        std::swap(left, right);
        predicate = CmpInst::getSwappedPredicate(predicate);
        break;
      default:
        break;
    }
    StringRef op;
    switch (predicate) {
      case CmpInst::ICMP_EQ: op = "ceq"; break;
      case CmpInst::ICMP_NE: op = "cneq"; break;
      case CmpInst::ICMP_SLT: op = "clt"; break;
      case CmpInst::ICMP_SGE: op = "cge"; break;
      case CmpInst::ICMP_ULT: op = "cltu"; break;
      case CmpInst::ICMP_UGE: op = "cgeu"; break;
      default: llvm_unreachable("invalid integer predicate");
    }
    out << op << " " << dest << ", " << left << ", " << right << "\n";
  }

  unsigned memoryWidth(Type *type) {
    if (auto *pointer = dyn_cast<PointerType>(type)) {
      if (pointer->getAddressSpace() == 0)
        return 32;
    } else if (type->isIntegerTy(8) || type->isIntegerTy(16) || type->isIntegerTy(32)) {
      return type->getIntegerBitWidth();
    }
    report_fatal_error("ZLX memory operands must be i8, i16, i32, or default-address-space pointers");
  }

  void stackTriplet(StringRef op, ArrayRef<std::string> registers) {
    out << op << " " << registers[0] << ".32, " << registers[1] << ".32, "
        << registers[2] << ".32\n";
  }

  void edge(BasicBlock *from, BasicBlock *to) {
    SmallVector<std::string, 3> pending;
    SmallVector<std::string, 6> destinations;
    for (auto &phi : to->phis()) {
      auto *source = phi.getIncomingValueForBlock(from);
      const auto &dest = regs.at(&phi);
      if (auto *integer = dyn_cast<ConstantInt>(source)) {
        if (integer->getBitWidth() > 32)
          report_fatal_error("ZLX expects PHI values of at most 32 bits");
        if (!integer->isZero()) {
          for (const auto &name : pending)
            out << "push32 " << name << "\n";
          pending.clear();
          out << "get ac, " << integer->getZExtValue() << "\npush32 ac\n";
          destinations.push_back(dest);
          continue;
        }
      }
      auto name = isa<ConstantInt, ConstantPointerNull>(source) ? std::string("z0") : reg(source);
      if (name == dest)
        continue;
      pending.push_back(name);
      destinations.push_back(dest);
      if (pending.size() == 3) {
        stackTriplet("push3", pending);
        pending.clear();
      }
    }
    for (const auto &name : pending)
      out << "push32 " << name << "\n";

    while (destinations.size() >= 3) {
      stackTriplet("pop3", ArrayRef<std::string>(destinations).take_back(3));
      destinations.resize(destinations.size() - 3);
    }
    for (const auto &name : reverse(destinations))
      out << "pop32 " << name << "\n";
  }

  void emitFunction(Function &f) {
    if (f.isDeclaration())
      return;
    regs.clear();labels.clear();occupied.clear();releaseAfter.clear();scratch.clear();
    findFoldedOffsets(f);
    unsigned selectIndex = 0;
    unsigned n=0;
    for(auto &a:f.args()){
      if(n==8)report_fatal_error("too many arguments");
      regs[&a]="a"+std::to_string(n++);
    }
    n=0;
    for(auto &bb:f){
      labels[&bb]=f.getName().str()+"_bb"+std::to_string(n++);
      for(auto &p:bb.phis())regs[&p]=temp();
    }
    out<<f.getName()<<":\n";
    for(auto &bb:f){
      out<<labels[&bb]<<":\n";
      for(auto &i:bb){
        if(auto *prev=i.getPrevNode())
          for(const auto &name:releaseAfter[prev])occupied.erase(name);
        if (isa<PHINode>(i) || !emitsCode(i))
          continue;
        auto dest = [&]() {
          auto users = effectiveUsers(i);
          const auto *next = i.getNextNode();
          while (next && !emitsCode(*next))
            next = next->getNextNode();
          auto name = users.size() == 1 && users.front() == next && !isa<PHINode>(next)
                          ? std::string("ac") : temp();
          if (name != "ac") {
            const Instruction *last = nullptr;
            for (const auto *use : users) {
              if (use->getParent() != &bb || isa<PHINode>(use)) {
                last = nullptr;
                break;
              }
              if (!last || last->comesBefore(use))
                last = use;
            }
            if (last)
              releaseAfter[last].push_back(name);
          }
          return regs[&i] = name;
        };
        if (auto *b = dyn_cast<BinaryOperator>(&i)) {
          binary(*b, dest());
        } else if (auto *r = dyn_cast<ReturnInst>(&i)) {
          if (auto *value = r->getReturnValue()) {
            if (auto *integer = dyn_cast<ConstantInt>(value)) {
              if (integer->getBitWidth() > 32)
                report_fatal_error("ZLX returns at most 32 bits");
              out << "get ac, " << integer->getZExtValue() << "\n";
            } else {
              move("ac", reg(value));
            }
          }
          out << "j rp\n";
        } else if (auto *gep = dyn_cast<GetElementPtrInst>(&i)) {
          auto offset = constantOffset(*gep);
          if (!offset)
            report_fatal_error("unsupported ZLX GEP");
          addOffset(dest(), reg(gep->getPointerOperand()), *offset);
        } else if (auto *load = dyn_cast<LoadInst>(&i)) {
          if (load->isAtomic() || load->getPointerAddressSpace() != 0)
            report_fatal_error("unsupported ZLX load");
          auto width = memoryWidth(load->getType());
          auto operand = address(load->getPointerOperand());
          out << "ld." << width << " " << dest() << ", " << operand << "\n";
        } else if (auto *store = dyn_cast<StoreInst>(&i)) {
          if (store->isAtomic() || store->getPointerAddressSpace() != 0)
            report_fatal_error("unsupported ZLX store");
          auto width = memoryWidth(store->getValueOperand()->getType());
          auto operand = address(store->getPointerOperand());
          auto value = reg(store->getValueOperand());
          out << "st." << width << " " << value << ", " << operand << "\n";
        } else if (auto *cast = dyn_cast<PtrToIntInst>(&i)) {
          if (!cast->getType()->isIntegerTy(32) || cast->getPointerAddressSpace() != 0)
            report_fatal_error("ZLX ptrtoint requires a default-address-space pointer and i32");
          move(dest(), reg(cast->getPointerOperand()));
        } else if (isa<ZExtInst, SExtInst, TruncInst>(i)) {
          auto *operand = i.getOperand(0);
          unsigned from = operand->getType()->getIntegerBitWidth();
          unsigned to = i.getType()->getIntegerBitWidth();
          if (from > 32 || to > 32)
            report_fatal_error("ZLX integer cast requires widths up to 32 bits");
          auto source = reg(operand);
          auto target = dest();
          if (isa<SExtInst>(i)) {
            out << "sext " << target << ", " << source << ", " << from << "\n";
          } else {
            mask(target, source, std::min(from, to));
          }
        } else if (auto *cmp = dyn_cast<ICmpInst>(&i)) {
          compare(*cmp, dest());
        } else if (auto *select = dyn_cast<SelectInst>(&i)) {
          const auto *compare = dyn_cast<ICmpInst>(select->getCondition());
          const auto *zeroTest = compare ? foldedZeroTest(*compare) : nullptr;
          auto condition = reg(zeroTest ? zeroTest : select->getCondition());
          const char *branch = zeroTest && compare->getPredicate() == CmpInst::ICMP_EQ ? "bneq" : "beq";
          auto whenTrue = reg(select->getTrueValue());
          auto whenFalse = reg(select->getFalseValue());
          auto target = dest();
          auto label = labels[&bb] + "_select" + std::to_string(selectIndex++);
          out << branch << " " << condition << ", z0, #" << label << "_false\n";
          move(target, whenTrue);
          out << "b #" << label << "_end\n" << label << "_false:\n";
          move(target, whenFalse);
          out << label << "_end:\n";
        }else if(auto *br=dyn_cast<BranchInst>(&i)){
          if(br->isConditional()){
            auto cond=reg(br->getCondition());auto skip=labels[&bb]+"_false";
            out<<"beq "<<cond<<", z0, #"<<skip<<"\n";edge(&bb,br->getSuccessor(0));
            out<<"b #"<<labels[br->getSuccessor(0)]<<"\n"<<skip<<":\n";
          }
          auto *to=br->getSuccessor(br->isConditional()?1:0);edge(&bb,to);
          if(to!=bb.getNextNode())out<<"b #"<<labels[to]<<"\n";
        }else report_fatal_error(Twine("unsupported ZLX instruction: ")+i.getOpcodeName());
        for (const auto &name : scratch)
          occupied.erase(name);
        scratch.clear();
      }
      for (const auto &name : releaseAfter[bb.getTerminator()])
        occupied.erase(name);
    }
  }
};
}
