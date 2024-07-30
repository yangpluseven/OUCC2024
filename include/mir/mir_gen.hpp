#ifndef MIR_MIR_GEN_HPP
#define MIR_MIR_GEN_HPP
#include "ir/module.hpp"
#include "mir.hpp"
#include "reg/register.hpp"

#include <iostream>

namespace mir {

class MachineFunction {
private:
  ir::Function *_func;
  std::vector<MIR *> _irs;
  const int _localSize, _iCallerNum, _fCallerNum;

public:
  int maxFuncParamNum = 0;

  MachineFunction(ir::Function *func, int localSize, int iCallerNum,
                  int fCallerNum)
      : _func(func), _localSize(localSize), _iCallerNum(iCallerNum),
        _fCallerNum(fCallerNum) {}

  void addMIR(MIR *mir) { _irs.push_back(mir); }

  [[nodiscard]] int getFCallerNum() const { return _fCallerNum; }
  [[nodiscard]] int getICallerNum() const { return _iCallerNum; }
  [[nodiscard]] int getLocalSize() const { return _localSize; }
  [[nodiscard]] std::vector<MIR *> getIrs() const { return _irs; }
  [[nodiscard]] std::string getName() const { return _func->getName(); }
  [[nodiscard]] std::string getRawName() const { return _func->getRawName(); }
  [[nodiscard]] std::string toString() const {
    std::stringstream ss;
    ss << _func->toString() << '\n';
    ss << "---------- mir ----------" << '\n';
    for (const auto mir : _irs) {
      ss << mir->toString() << '\n';
    }
    return ss.str();
  }

  void print() const { std::cout << toString(); }
};

class MIRGenerator {
private:
  ir::Module *_module;
  std::unordered_map<std::string, MachineFunction *> _mFuncs;
  bool _isProcessed = false;

  MachineFunction *funcToMir(ir::Function *func);

  void llvmToMir() {
    for (auto func : _module->getFunctions()) {
      if (!func->isDeclare()) {
        _mFuncs[func->getName()] = funcToMir(func);
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
  explicit MIRGenerator(ir::Module *module) : _module(module) {}

  void checkIfIsProcessed() {
    if (_isProcessed) {
      return;
    }
    _isProcessed = true;
    llvmToMir();
  }

  std::unordered_map<std::string, MachineFunction *> getFuncs() {
    checkIfIsProcessed();
    return _mFuncs;
  }

  std::unordered_set<ir::GlobalVariable *> getGlobals() {
    checkIfIsProcessed();
    std::unordered_set<ir::GlobalVariable *> s;
    for (const auto glob : _module->getGlobals()) {
      s.insert(glob);
    }
    return s;
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
