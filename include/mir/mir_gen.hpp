#ifndef MIR_MIR_GEN_HPP
#define MIR_MIR_GEN_HPP
#include "ir/module.hpp"
#include "reg/register.hpp"

namespace mir {

class MachineFunction {};

class MIRGenerator {
private:
  ir::Module *module;
  std::unordered_map<std::string, MachineFunction *> mFuncs;
  bool isProcessed = false;

  MachineFunction *funcToMir(ir::Function *func);

  void llvmToMir() {
    for (auto func : module->getFunctions()) {
      if (!func->isDeclare()) {
        mFuncs[func->getName()] = funcToMir(func);
      }
    }
  }

  static std::pair<int, int> getCallerNumbers(ir::Function const *func) {
    int iSize = 0, fSize = 0;
    for (auto arg : func->getArgs()) {
      if (arg->getType() == ir::BasicType::FLOAT) {
        fSize = std::min(fSize + 1,
                         static_cast<int>(reg::MReg::callerFRegs.size()));
      } else {
        iSize = std::min(iSize + 1,
                         static_cast<int>(reg::MReg::callerIRegs.size()));
      }
    }
    return {iSize, fSize};
  }

public:
  explicit MIRGenerator(ir::Module *module) : module(module) {}

  void checkIfIsProcessed() {
    if (isProcessed) {
      return;
    }
    isProcessed = true;
    llvmToMir();
  }

  std::unordered_map<std::string, MachineFunction *> getFuncs() {
    checkIfIsProcessed();
    return mFuncs;
  }

  std::vector<ir::GlobalVariable *> getGlobals() {
    checkIfIsProcessed();
    return module->getGlobals();
  }

  std::pair<int, std::unordered_map<ir::AllocaInst *, int>>
  calcLocalOffsets(ir::BasicBlock *block);
  std::unordered_map<ir::Argument *, std::pair<bool, int>>
  calcArgOffsets(std::vector<ir::Argument *> &args);
};

} // namespace mir

#endif // MIR_MIR_GEN_HPP
