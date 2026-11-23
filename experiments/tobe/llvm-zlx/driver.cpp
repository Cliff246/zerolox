#include "llvm/IRReader/IRReader.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/Support/SourceMgr.h"
#include "ZLX.cpp"

int main(int argc, char **argv) {
  if (argc != 2) {
    llvm::errs() << "usage: zlx-emit input.ll\n";
    return 1;
  }
  llvm::LLVMContext context;
  llvm::SMDiagnostic diagnostic;
  auto module = llvm::parseIRFile(argv[1], diagnostic, context);
  if (!module) {
    diagnostic.print(argv[0], llvm::errs());
    return 1;
  }
  llvm::legacy::PassManager passes;
  passes.add(new Emit(llvm::outs()));
  passes.run(*module);
}
