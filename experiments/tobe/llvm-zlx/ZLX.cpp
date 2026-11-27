#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"
#include <cstdint>
#include <map>
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

  std::string materialize(uint32_t value) {
    if (value == 0)
      return "z0";
    auto name = temp();
    scratch.push_back(name);
    out << "get " << name << ", " << value << "\n";
    return name;
  }

  std::string reg(Value *value) {
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

  void addOffset(StringRef dest, StringRef base, int64_t offset) {
    if (offset >= -1024 && offset <= 1023) {
      out << "addi " << dest << ", " << base << ", " << offset << "\n";
    } else {
      auto value = materialize(static_cast<uint32_t>(offset));
      out << "add " << dest << ", " << base << ", " << value << "\n";
    }
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
        if(isa<PHINode>(i))continue;
        auto dest=[&](){
          bool adjacent=i.hasOneUse() && *i.user_begin()==i.getNextNode() && !isa<PHINode>(*i.user_begin());
          auto name=adjacent?std::string("ac"):temp();
          if(name!="ac"){
            Instruction *last=nullptr;
            for(auto *user:i.users()){
              auto *use=dyn_cast<Instruction>(user);
              if(!use || use->getParent()!=&bb || isa<PHINode>(use)){
                last=nullptr;
                break;
              }
              if(!last || last->comesBefore(use))last=use;
            }
            if(last)releaseAfter[last].push_back(name);
          }
          return regs[&i]=name;
        };
        if(auto *b=dyn_cast<BinaryOperator>(&i)){
          if(!i.getType()->isIntegerTy(32))report_fatal_error("expected i32 ALU");
          auto x=reg(b->getOperand(0)),d=dest();
          if(auto *k=dyn_cast<ConstantInt>(b->getOperand(1))){
            if (i.getOpcode() == Instruction::Add) {
              addOffset(d, x, k->getSExtValue());
            } else if (i.getOpcode() == Instruction::LShr && k->getZExtValue() < 32) {
              out << "srli " << d << ", " << x << ", " << k->getZExtValue() << "\n";
            } else {
              auto y = reg(k);
              StringRef op = i.getOpcodeName();
              if (op != "sub" && op != "and" && op != "or" && op != "xor")
                report_fatal_error("unsupported ZLX immediate");
              out << op << " " << d << ", " << x << ", " << y << "\n";
            }
          }else{
            auto y=reg(b->getOperand(1)); StringRef op=i.getOpcodeName();
            if(op!="add" && op!="sub" && op!="and" && op!="or" && op!="xor")report_fatal_error("unsupported ALU");
            if((op=="and"||op=="or"||op=="xor") && d=="ac" && (x=="ac"||y=="ac"))out<<op<<"a "<<(x=="ac"?y:x)<<"\n";
            else out<<op<<" "<<d<<", "<<x<<", "<<y<<"\n";
          }
        }else if(auto *r=dyn_cast<ReturnInst>(&i)){
          if (auto *value = r->getReturnValue()) {
            auto name = reg(value);
            if (name != "ac")
              out << "mv ac, " << name << "\n";
          }
          out << "j rp\n";
        }else if(auto *g=dyn_cast<GetElementPtrInst>(&i)){
          if (g->getPointerAddressSpace() != 0)
            report_fatal_error("ZLX GEP requires the default address space");
          const auto &layout = f.getParent()->getDataLayout();
          APInt offset(layout.getIndexSizeInBits(g->getPointerAddressSpace()), 0);
          if (!g->accumulateConstantOffset(layout, offset))
            report_fatal_error("nonconstant ZLX GEP");
          auto base = reg(g->getPointerOperand());
          addOffset(dest(), base, offset.getSExtValue());
        }else if(auto *l=dyn_cast<LoadInst>(&i)){
          if (l->isAtomic() || l->getPointerAddressSpace() != 0)
            report_fatal_error("unsupported ZLX load");
          const auto width = memoryWidth(l->getType());
          auto base = reg(l->getPointerOperand());
          out << "ld." << width << " " << dest() << ", [" << base << "]\n";
        }else if(auto *s=dyn_cast<StoreInst>(&i)){
          if (s->isAtomic() || s->getPointerAddressSpace() != 0)
            report_fatal_error("unsupported ZLX store");
          const auto width = memoryWidth(s->getValueOperand()->getType());
          auto base = reg(s->getPointerOperand());
          auto value = reg(s->getValueOperand());
          out << "st." << width << " " << value << ", [" << base << "]\n";
        }else if(auto *cast=dyn_cast<PtrToIntInst>(&i)){
          if (!cast->getType()->isIntegerTy(32) || cast->getPointerAddressSpace() != 0)
            report_fatal_error("ZLX ptrtoint requires a default-address-space pointer and i32");
          auto source = reg(cast->getPointerOperand());
          auto target = dest();
          if (source != target)
            out << "mv " << target << ", " << source << "\n";
        }
        else if(auto *cmp=dyn_cast<ICmpInst>(&i)){
          if(!cmp->isEquality())report_fatal_error("expected equality");
          auto x=reg(i.getOperand(0)),y=reg(i.getOperand(1)),d=dest();
          out<<(cmp->getPredicate()==CmpInst::ICMP_EQ?"ceq ":"cneq ")<<d<<", "<<x<<", "<<y<<"\n";
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
