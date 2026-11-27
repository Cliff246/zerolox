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
  if (module->getDataLayoutStr().empty())
    module->setDataLayout("e-p:32:32-i64:64-n32-S64");
  const auto &layout = module->getDataLayout();
  if (!layout.isLittleEndian() || layout.getPointerSizeInBits() != 32 ||
      layout.getIndexSizeInBits(0) != 32) {
    llvm::errs() << "zlx-emit requires little-endian IR with 32-bit pointers and indices\n";
    return 1;
  }
  Emit emitter(llvm::outs());
  for (auto &function : *module)
    emitter.emitFunction(function);
}
