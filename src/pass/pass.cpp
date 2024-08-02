#include "pass.hpp"

namespace pass {

bool DCEPass::runOnFunction(ir::Function *function) {
  bool modified = false;
  for (const auto block : *function) {
    for (int i = 0; i + 1 < block->size(); i++) {
      const auto inst = block->get(i);
      const auto callInst = dynamic_cast<ir::CallInst *>(inst);
      const auto storeInst = dynamic_cast<ir::StoreInst *>(inst);
      if (!(callInst || storeInst) && inst->getUses().empty()) {
        block->remove(i);
        i--;
        modified = true;
      }
    }
  }
  return modified;
}

bool BranchOptPass::runOnFunction(ir::Function *function) {
  bool modified = false;
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
                  phiNode->remove(j);
                  j--;
                }
              }
            }
          }
        }
      }
      function->remove(i);
      i--;
      modified = true;
    } else if (uses.size() == 1) {
      auto it = uses.begin();
      const auto branchInst = dynamic_cast<ir::BranchInst *>((*it)->getUser());
      if (!branchInst->isConditional()) {
        ir::BasicBlock *prevBlock = branchInst->getBlock();
        prevBlock->remove(static_cast<int>(prevBlock->size()) - 1);
        for (const auto inst : *block) {
          inst->setBlock(prevBlock);
          prevBlock->add(inst);
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
        function->remove(i);
        i--;
        modified = true;
      }
    }
  }
  for (int i = 0; i < function->size(); i++) {
    const ir::BasicBlock *block = function->get(i);
    const auto branchInst = dynamic_cast<ir::BranchInst *>(block->getLast());
    if (branchInst && branchInst->isConditional()) {
      if (const auto condValue = dynamic_cast<ir::ConstantNumber *>(
              branchInst->getOperand<ir::Value>(0))) {
        branchInst->remove(0);
        ir::Use *use = branchInst->remove(condValue->intValue());
        const auto nextBlock = dynamic_cast<ir::BasicBlock *>(use->getValue());
        for (const auto inst : *nextBlock) {
          if (const auto phiNode = dynamic_cast<ir::PHINode *>(inst)) {
            for (int j = 0; j < phiNode->size(); j++) {
              if (phiNode->getBlockValue(j).first == block) {
                phiNode->remove(j);
                j--;
              }
            }
          }
        }
        modified = true;
      }
    }
  }
  return modified;
}

bool ConstPropPass::runOnFunction(ir::Function *function) { return false; }

bool PromotePass::runOnFunction(ir::Function *function) { return false; }

} // namespace pass
