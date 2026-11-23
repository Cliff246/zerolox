#include "llvm/IR/Constants.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Pass.h"
#include <map>
#include <set>
using namespace llvm;
namespace {
struct Emit : FunctionPass {
  static char ID;
  raw_ostream &out;
  Emit(raw_ostream &out):FunctionPass(ID),out(out){}
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
  std::string reg(Value *v){
    if(auto *k=dyn_cast<ConstantInt>(v)){
      if(k->isZero())return "z0";
      out<<"get ac, "<<k->getZExtValue()<<"\n";return "ac";
    }
    return regs.at(v);
  }
  void edge(BasicBlock *from,BasicBlock *to){
    SmallVector<std::pair<std::string,std::string>> copies;
    for(auto &phi:to->phis()){
      auto src=reg(phi.getIncomingValueForBlock(from));
      if(src!=regs.at(&phi)){
        out<<"push32 "<<src<<"\n";copies.emplace_back(src,regs.at(&phi));
      }
    }
    for(auto &p:reverse(copies))out<<"pop32 "<<p.second<<"\n";
  }
  bool runOnFunction(Function &f) override {
    if(f.isDeclaration())return false;
    regs.clear();labels.clear();occupied.clear();releaseAfter.clear();
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
              if(!use || use->getParent()!=&bb || isa<PHINode>(use) || isa<PtrToIntInst>(use)){
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
            StringRef op;
            switch(i.getOpcode()){case Instruction::Add:op="addi";break;case Instruction::LShr:op="srli";break;default:report_fatal_error("unsupported immediate");}
            out<<op<<" "<<d<<", "<<x<<", "<<k->getSExtValue()<<"\n";
          }else{
            auto y=reg(b->getOperand(1)); StringRef op=i.getOpcodeName();
            if(op!="add" && op!="sub" && op!="and" && op!="or" && op!="xor")report_fatal_error("unsupported ALU");
            if((op=="and"||op=="or"||op=="xor") && d=="ac" && (x=="ac"||y=="ac"))out<<op<<"a "<<(x=="ac"?y:x)<<"\n";
            else out<<op<<" "<<d<<", "<<x<<", "<<y<<"\n";
          }
        }else if(auto *r=dyn_cast<ReturnInst>(&i)){
          if(!r->getReturnValue()){out<<"ret z0\n";continue;}
          auto v=reg(r->getReturnValue());
          if(v!="ac")out<<"mv ac, "<<v<<"\n";
          out<<"ret ac\n";
        }else if(auto *g=dyn_cast<GetElementPtrInst>(&i)){
          APInt off(32,0);if(!g->accumulateConstantOffset(f.getDataLayout(),off))report_fatal_error("nonconstant GEP");
          auto p=reg(g->getPointerOperand());auto d=dest();out<<"addi "<<d<<", "<<p<<", "<<off.getSExtValue()<<"\n";
        }else if(auto *l=dyn_cast<LoadInst>(&i)){
          if(!l->getType()->isIntegerTy(8)||l->isAtomic())report_fatal_error("expected byte load");
          auto p=reg(l->getPointerOperand());auto d=dest();out<<"ld.8 "<<d<<", ["<<p<<"]\n";
        }else if(auto *s=dyn_cast<StoreInst>(&i)){
          if(!s->getValueOperand()->getType()->isIntegerTy(8)||s->isAtomic())report_fatal_error("expected byte store");
          auto p=reg(s->getPointerOperand()),v=reg(s->getValueOperand());out<<"st.8 "<<v<<", ["<<p<<"]\n";
        }else if(isa<PtrToIntInst>(i)){regs[&i]=reg(i.getOperand(0));}
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
      }
    }
    return false;
  }
};
char Emit::ID;
class Machine:public TargetMachine{
public:
 Machine(const Target&t,const Triple&tt,StringRef cpu,StringRef fs,const TargetOptions&o,std::optional<Reloc::Model>,std::optional<CodeModel::Model>,CodeGenOptLevel,bool):TargetMachine(t,"e-p:32:32-i64:64-n32-S64",tt,cpu,fs,o){}
 bool addPassesToEmitFile(PassManagerBase &pm,raw_pwrite_stream &out,raw_pwrite_stream*,CodeGenFileType type,bool,MachineModuleInfoWrapperPass*)override{
   if(type!=CodeGenFileType::AssemblyFile)return true;
   pm.add(new Emit(out));return false;
 }
};
Target target;
RegisterTarget<Triple::UnknownArch> registration(target,"zlx","ZeroLox ZLX-1","ZLX");
RegisterTargetMachine<Machine> machine(target);
}
