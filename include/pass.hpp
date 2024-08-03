#ifndef PASS_PASS_HPP
#define PASS_PASS_HPP

#include "module.hpp"

#include <stack>

namespace pass {

class Pass {
protected:
  const ir::Module *module;

public:
  explicit Pass(const ir::Module *module) { this->module = module; }

  virtual ~Pass() = default;

  virtual bool run() = 0;
};

class FunctionPass : public Pass {
public:
  explicit FunctionPass(const ir::Module *module) : Pass(module) {}

  bool run() override {
    bool modified = false;
    for (auto function : module->getFunctions()) {
      modified |= runOnFunction(function);
    }
    return modified;
  }

  virtual bool runOnFunction(ir::Function *function) = 0;
};

class BranchOptPass : public FunctionPass {
public:
  explicit BranchOptPass(const ir::Module *module) : FunctionPass(module) {}

  bool runOnFunction(ir::Function *function) override;
};

class ConstPropPass : public FunctionPass {
public:
  explicit ConstPropPass(const ir::Module *module) : FunctionPass(module) {}

  bool runOnFunction(ir::Function *function) override;
};

class DCEPass : public FunctionPass {
public:
  explicit DCEPass(const ir::Module *module) : FunctionPass(module) {}

  ~DCEPass() override = default;

  bool runOnFunction(ir::Function *function) override;
};

class PromotePass : public FunctionPass {
private:
  std::unordered_map<ir::BasicBlock *,
                     std::unordered_map<ir::AllocaInst *, ir::PHINode *>>
      _globalPhiMap;
  std::unordered_set<ir::AllocaInst *>
  static analyzePromoteAllocaInsts(ir::Function *function);
  static bool isAllocaPromotable(ir::AllocaInst *allocaInst);
  void insertPhi(ir::Function *func,
                 std::unordered_map<ir::BasicBlock *,
                                    std::unordered_set<ir::BasicBlock *>> &df,
                 std::unordered_set<ir::AllocaInst *> &allocaInsts);
  void rename(
      ir::BasicBlock *block,
      std::unordered_map<ir::AllocaInst *, std::stack<ir::Value *>> &replaceMap,
      std::unordered_set<ir::AllocaInst *> &allocaInsts,
      std::unordered_map<ir::BasicBlock *, std::unordered_set<ir::BasicBlock *>>
          &domTree);
  void prunePhi(ir::Function *func);

public:
  explicit PromotePass(const ir::Module *module) : FunctionPass(module) {}

  bool runOnFunction(ir::Function *function) override;
};
} // namespace pass

#endif // PASS_PASS_HPP