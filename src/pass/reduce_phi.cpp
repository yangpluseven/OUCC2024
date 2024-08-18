#include "pass.hpp"

namespace pass {

bool ReducePhi::onFunction(ir::Function *function) {
  if (function->isDeclare()) {
    return false;
  }
  bool changed = false;
  std::vector<ir::Instruction *> toErase;
  for (const auto block : *function) {
    toErase.clear();
    for (const auto inst : *block) {
      if (const auto phiNode = dynamic_cast<ir::PHINode *>(inst)) {
        if (phiNode->size() > 1) {
          continue;
        }
        const auto value = phiNode->getOperand<ir::Value>(0);
        if (const auto constant = dynamic_cast<ir::Constant *>(value)) {
          if (const auto arr = dynamic_cast<ir::ConstantArray *>(constant)) {
            // TODO
            continue;
          }
          // const auto allocaInst = new ir::AllocaInst(block,
          // const auto storeInst = new constant->getType());
          // ir::StoreInst(block, constant, allocaInst);
          // phiNode->replaceAllUseAs(storeInst);
          // block->insert(0, allocaInst);
          // block->insert(phiNode, storeInst);
          // block->erase(phiNode);

          phiNode->replaceAllUseAs(constant);
          toErase.push_back(phiNode);
          changed = true;
        } else if (const auto glob =
                       dynamic_cast<ir::GlobalVariable *>(value)) {
          phiNode->replaceAllUseAs(glob);
          toErase.push_back(phiNode);
          changed = true;
        } else if (const auto valueInst =
                       dynamic_cast<ir::Instruction *>(value)) {
          phiNode->replaceAllUseAs(valueInst);
          toErase.push_back(phiNode);
          changed = true;
        } else {
          throw std::runtime_error(
              "Unsupported value type in ReducePhi::onFunction");
        }
      }
    }
    for (auto *inst : toErase) {
      block->erase(inst);
    }
  }
  return changed;
}

} // namespace pass