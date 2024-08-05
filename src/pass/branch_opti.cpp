#include "pass.hpp"

namespace pass {

bool BranchOpt::onFunction(ir::Function *function) {
  bool changed = false;
  for (int i = 1; i < function->size(); i++) {
    const auto block = function->get(i);
    const auto &uses = block->getUses();
    if (uses.empty()) {
      if (const auto branchInst =
              dynamic_cast<ir::BranchInst *>(block->getLast())) {
        std::unordered_set<ir::BasicBlock *> nextBlocks;
        if (branchInst->isConditional()) {
          nextBlocks.insert(branchInst->getOperand<ir::BasicBlock>(1));
          nextBlocks.insert(branchInst->getOperand<ir::BasicBlock>(2));
        } else {
          nextBlocks.insert(branchInst->getOperand<ir::BasicBlock>(0));
        }
        for (const auto nextBlock : nextBlocks) {
          for (const auto inst : *nextBlock) {
            if (const auto phiNode = dynamic_cast<ir::PHINode *>(inst)) {
              for (int j = 0; j < phiNode->size(); j++) {
                if (phiNode->getBlockValue(j).first == block) {
                  phiNode->erase(j);
                  j--;
                }
              }
            }
          }
        }
      }
      function->erase(i);
      i--;
      changed = true;
    } else if (uses.size() == 1) {
      auto it = uses.begin();
      const auto branchInst = dynamic_cast<ir::BranchInst *>((*it)->getUser());
      if (!branchInst->isConditional()) {
        ir::BasicBlock *prevBlock = branchInst->getBlock();
        prevBlock->remove(static_cast<int>(prevBlock->size()) - 1);
        for (const auto inst : *block) {
          inst->setBlock(prevBlock);
          prevBlock->insert(inst);
        }
        if (const auto nextBranchInst =
                dynamic_cast<ir::BranchInst *>(block->getLast())) {
          std::unordered_set<ir::BasicBlock *> nextBlocks;
          if (nextBranchInst->isConditional()) {
            nextBlocks.insert(nextBranchInst->getOperand<ir::BasicBlock>(1));
            nextBlocks.insert(nextBranchInst->getOperand<ir::BasicBlock>(2));
          } else {
            nextBlocks.insert(nextBranchInst->getOperand<ir::BasicBlock>(0));
          }
          for (const auto nextBlock : nextBlocks) {
            for (const auto inst : *nextBlock) {
              if (const auto phiNode = dynamic_cast<ir::PHINode *>(inst)) {
                for (int j = 0; j < phiNode->size(); j++) {
                  if (phiNode->getBlockValue(j).first == block) {
                    phiNode->setBlockValue(j, prevBlock);
                  }
                }
              }
            }
          }
        }
        function->erase(i);
        i--;
        changed = true;
      }
    }
  }
  for (int i = 0; i < function->size(); i++) {
    const ir::BasicBlock *block = function->get(i);
    const auto branchInst = dynamic_cast<ir::BranchInst *>(block->getLast());
    if (branchInst && branchInst->isConditional()) {
      if (const auto condValue = dynamic_cast<ir::ConstantNumber *>(
              branchInst->getOperand<ir::Value>(0))) {
        branchInst->erase(0);
        ir::Use *use = branchInst->erase(condValue->intValue());
        const auto nextBlock = dynamic_cast<ir::BasicBlock *>(use->getValue());
        for (const auto inst : *nextBlock) {
          if (const auto phiNode = dynamic_cast<ir::PHINode *>(inst)) {
            for (int j = 0; j < phiNode->size(); j++) {
              if (phiNode->getBlockValue(j).first == block) {
                phiNode->erase(j);
                j--;
              }
            }
          }
        }
        changed = true;
      }
    }
  }
  return changed;
}

} // namespace pass