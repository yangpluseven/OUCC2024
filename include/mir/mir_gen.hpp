#ifndef MIR_MIR_GEN_HPP
#define MIR_MIR_GEN_HPP
#include "ir/module.hpp"
#include "mir.hpp"
#include "reg/register.hpp"

#include <iostream>

namespace mir {

class MachineFunction {
private:
  ir::Function *func;
  std::vector<MIR *> irs;
  const int localSize, iCallerNum, fCallerNum;

public:
  int maxFuncParamNum = 0;

  MachineFunction(ir::Function *func, int localSize, int iCallerNum,
                  int fCallerNum)
      : func(func), localSize(localSize), iCallerNum(iCallerNum),
        fCallerNum(fCallerNum) {}

  void addMIR(MIR *mir) { irs.push_back(mir); }

  [[nodiscard]] int getFCallerNum() const { return fCallerNum; }
  [[nodiscard]] int getICallerNum() const { return iCallerNum; }
  [[nodiscard]] int getLocalSize() const { return localSize; }
  [[nodiscard]] std::vector<MIR *> getIrs() const { return irs; }
  [[nodiscard]] std::string getName() const { return func->getName(); }
  [[nodiscard]] std::string getRawName() const { return func->getRawName(); }
  [[nodiscard]] std::string toString() const {
    std::stringstream ss;
    ss << func->toString() << '\n';
    ss << "---------- mir ----------" << '\n';
    for (auto const mir : irs) {
      ss << mir->toString() << '\n';
    }
    return ss.str();
  }

  void print() const {
    std::cout << toString();
  }


};

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
                         static_cast<int>(reg::MReg::F_CALLER_REGS.size()));
      } else {
        iSize = std::min(iSize + 1,
                         static_cast<int>(reg::MReg::I_CALLER_REGS.size()));
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

class FakeMvInst : ir::Instruction {
public:
  FakeMvInst(ir::BasicBlock *block, ir::Value *target, ir::Value *src)
      : ir::Instruction(block, ir::BasicType::VOID, {target, src}) {}
};

} // namespace mir

#endif // MIR_MIR_GEN_HPP
