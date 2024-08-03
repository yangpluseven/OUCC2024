#include "pass.hpp"
#include "dom_tree.hpp"

#include <queue>

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

bool ConstPropPass::runOnFunction(ir::Function *function) {
  bool modified = false;
  for (const auto block : *function) {
    for (int i = 0; i < block->size(); i++) {
      const auto inst = block->get(i);
      if (const auto binaryOperator =
              dynamic_cast<ir::BinaryOperator *>(inst)) {
        const auto value1 = dynamic_cast<ir::ConstantNumber *>(
            binaryOperator->getOperand<ir::Value>(0));
        if (value1) {
          const auto value2 = dynamic_cast<ir::ConstantNumber *>(
              binaryOperator->getOperand<ir::Value>(1));
          if (value2) {
            ir::ConstantNumber *newValue = nullptr;
            switch (binaryOperator->op) {
            case ir::BinaryOperator::ADD:
            case ir::BinaryOperator::FADD:
              newValue = value1->add(value2);
              break;
            case ir::BinaryOperator::SUB:
            case ir::BinaryOperator::FSUB:
              newValue = value1->sub(value2);
              break;
            case ir::BinaryOperator::MUL:
            case ir::BinaryOperator::FMUL:
              newValue = value1->mul(value2);
              break;
            case ir::BinaryOperator::SDIV:
            case ir::BinaryOperator::FDIV:
              newValue = value1->div(value2);
              break;
            case ir::BinaryOperator::SREM:
              newValue = value1->rem(value2);
              break;
            case ir::BinaryOperator::XOR:
              newValue = value1->exor(value2);
            }
            if (!newValue) {
              throw std::runtime_error("Unsupported binary operator in "
                                       "ConstPropPass::runOnFunction");
            }
            binaryOperator->replaceAllUseAs(newValue);
            block->remove(i);
            i--;
            modified = true;
          }
        }
      } else if (const auto cmpInst = dynamic_cast<ir::CmpInst *>(inst)) {
        const auto value1 = dynamic_cast<ir::ConstantNumber *>(
            cmpInst->getOperand<ir::Value>(0));
        if (value1) {
          const auto value2 = dynamic_cast<ir::ConstantNumber *>(
              cmpInst->getOperand<ir::Value>(1));
          if (value2) {
            ir::ConstantNumber *newValue = nullptr;
            switch (cmpInst->getCond()) {
            case ir::CmpInst::EQ:
            case ir::CmpInst::OEQ:
              newValue = value1->eq(value2);
              break;
            case ir::CmpInst::NE:
            case ir::CmpInst::UNE:
              newValue = value1->ne(value2);
              break;
            case ir::CmpInst::SGE:
            case ir::CmpInst::OGE:
              newValue = value1->ge(value2);
              break;
            case ir::CmpInst::SGT:
            case ir::CmpInst::OGT:
              newValue = value1->gt(value2);
              break;
            case ir::CmpInst::SLE:
            case ir::CmpInst::OLE:
              newValue = value1->le(value2);
              break;
            case ir::CmpInst::SLT:
            case ir::CmpInst::OLT:
              newValue = value1->lt(value2);
              break;
            }
            if (!newValue) {
              throw std::runtime_error("Unsupported cmp instruction in "
                                       "ConstPropPass::runOnFunction");
            }
            cmpInst->replaceAllUseAs(newValue);
            block->remove(i);
            i--;
            modified = true;
          }
        }
      }
    }
  }
  return modified;
}

std::unordered_set<ir::AllocaInst *>
PromotePass::analyzePromoteAllocaInsts(ir::Function *function) {
  std::unordered_set<ir::AllocaInst *> allocaInsts;
  for (const auto block : *function) {
    for (const auto inst : *block) {
      if (const auto allocaInst = dynamic_cast<ir::AllocaInst *>(inst)) {
        if (isAllocaPromotable(allocaInst)) {
          allocaInsts.insert(allocaInst);
        }
      }
    }
  }
  return std::move(allocaInsts);
}

void PromotePass::insertPhi(
    ir::Function *func,
    std::unordered_map<ir::BasicBlock *, std::unordered_set<ir::BasicBlock *>>
        &df,
    std::unordered_set<ir::AllocaInst *> &allocaInsts) {
  std::unordered_set<ir::AllocaInst *> globals;
  std::unordered_map<ir::AllocaInst *, std::unordered_set<ir::BasicBlock *>>
      blocks;
  for (const auto block : *func) {
    std::unordered_set<ir::AllocaInst *> varKill;
    for (const auto inst : *block) {
      if (const auto loadInst = dynamic_cast<ir::LoadInst *>(inst)) {
        const auto allocaInst =
            dynamic_cast<ir::AllocaInst *>(loadInst->getOperand<ir::Value>(0));
        if (allocaInst && allocaInsts.find(allocaInst) != allocaInsts.end() &&
            varKill.find(allocaInst) == varKill.end()) {
          globals.insert(allocaInst);
        }
      } else if (const auto storeInst = dynamic_cast<ir::StoreInst *>(inst)) {
        const auto allocaInst =
            dynamic_cast<ir::AllocaInst *>(storeInst->getOperand<ir::Value>(1));
        if (allocaInst && allocaInsts.find(allocaInst) != allocaInsts.end()) {
          varKill.insert(allocaInst);
          blocks[allocaInst].insert(block);
        }
      }
    }
  }
  for (const auto global : globals) {
    std::queue<ir::BasicBlock *> workList;
    for (const auto b : blocks.at(global)) {
      workList.push(b);
    }
    while (!workList.empty()) {
      const auto block = workList.front();
      workList.pop();
      for (const auto d : df.at(block)) {
        if (_globalPhiMap.at(d).find(global) == _globalPhiMap.at(d).end()) {
          _globalPhiMap.at(d)[global] =
              new ir::PHINode(d, global->getType()->baseType());
          workList.push(d);
        }
      }
    }
  }
}

void PromotePass::rename(
    ir::BasicBlock *block,
    std::unordered_map<ir::AllocaInst *, std::stack<ir::Value *>> &replaceMap,
    std::unordered_set<ir::AllocaInst *> &allocaInsts,
    std::unordered_map<ir::BasicBlock *, std::unordered_set<ir::BasicBlock *>>
        &domTree) {
  std::unordered_map<ir::AllocaInst *, int> counter;
  for (const auto &entry : _globalPhiMap.at(block)) {
    const auto allocaInst = entry.first;
    replaceMap[allocaInst].push(entry.second);
    counter[allocaInst] = counter[allocaInst] + 1;
  }
  for (int i = 0; i < block->size(); i++) {
    const auto inst = block->get(i);
    if (const auto storeInst = dynamic_cast<ir::StoreInst *>(inst)) {
      const auto allocaInst =
          dynamic_cast<ir::AllocaInst *>(storeInst->getOperand<ir::Value>(1));
      if (allocaInst && allocaInsts.find(allocaInst) != allocaInsts.end()) {
        block->remove(i);
        i--;
        replaceMap[allocaInst].push(storeInst->getOperand<ir::Value>(0));
        counter[allocaInst] = counter[allocaInst] + 1;
      }
    } else if (const auto loadInst = dynamic_cast<ir::LoadInst *>(inst)) {
      const auto allocaInst =
          dynamic_cast<ir::AllocaInst *>(loadInst->getOperand<ir::Value>(0));
      if (allocaInst && allocaInsts.find(allocaInst) != allocaInsts.end()) {
        block->remove(i);
        i--;
        if (replaceMap.find(allocaInst) != replaceMap.end()) {
          loadInst->replaceAllUseAs(replaceMap.at(allocaInst).top());
        } else {
          loadInst->replaceAllUseAs(
              allocaInst->getType()->baseType() == ir::BasicType::I32
                  ? new ir::ConstantNumber(model::Number(0))
                  : new ir::ConstantNumber(model::Number(0.0f)));
        }
      }
    }
  }
  std::vector<ir::BasicBlock *> nextBlocks;
  if (const auto branchInst =
          dynamic_cast<ir::BranchInst *>(block->getLast())) {
    if (branchInst->isConditional()) {
      nextBlocks.push_back(branchInst->getOperand<ir::BasicBlock>(1));
      nextBlocks.push_back(branchInst->getOperand<ir::BasicBlock>(2));
    } else {
      nextBlocks.push_back(branchInst->getOperand<ir::BasicBlock>(0));
    }
  }
  for (const auto nextBlock : nextBlocks) {
    const auto &phiMap = _globalPhiMap.at(nextBlock);
    for (const auto &phiEntry : phiMap) {
      const auto allocaInst = phiEntry.first;
      const auto phiNode = phiMap.at(allocaInst);
      if (replaceMap.find(allocaInst) != replaceMap.end()) {
        phiNode->add(block,
                     new ir::Use(phiNode, replaceMap.at(allocaInst).top()));
      } else {
        phiNode->add(
            block,
            new ir::Use(phiNode,
                        allocaInst->getType()->baseType() == ir::BasicType::I32
                            ? new ir::ConstantNumber(model::Number(0))
                            : new ir::ConstantNumber(model::Number(0.0f))));
      }
    }
  }
  if (domTree.find(block) != domTree.end()) {
    for (const auto nextBlock : domTree.at(block)) {
      rename(nextBlock, replaceMap, allocaInsts, domTree);
    }
  }
  for (const auto &entry : counter) {
    for (int i = 0; i < entry.second; i++) {
      replaceMap.at(entry.first).pop();
    }
    if (replaceMap.at(entry.first).empty()) {
      replaceMap.erase(entry.first);
    }
  }
}

bool PromotePass::isAllocaPromotable(ir::AllocaInst *allocaInst) {
  const auto type = allocaInst->getType();
  if (const auto ptrType = dynamic_cast<ir::PointerType *>(type)) {
    if (dynamic_cast<ir::BasicType *>(ptrType->baseType())) {
      return true;
    }
  }
  return false;
}

void PromotePass::prunePhi(ir::Function *func) {
  bool toContinue;
  do {
    toContinue = false;
    std::queue<ir::PHINode *> workList;
    std::unordered_set<ir::PHINode *> marked;
    for (const auto block : *func) {
      bool removed = false;
      auto &phiMap = _globalPhiMap.at(block);
      for (auto it = phiMap.begin(); it != phiMap.end();) {
        if (it->second->getUses().empty()) {
          it = phiMap.erase(it);
          removed = true;
        } else {
          ++it;
        }
      }
      toContinue |= removed;
    }
    for (int i = static_cast<int>(func->size()) - 1; i >= 0; i--) {
      const auto block = func->get(i);
      for (const auto &p : _globalPhiMap.at(block)) {
        const auto phiNode = p.second;
        workList.push(phiNode);
        marked.insert(phiNode);
      }
    }
    while (!workList.empty()) {
      const auto phiNode = workList.front();
      workList.pop();
      marked.erase(phiNode);
      ir::Value *replaceValue = nullptr;
      if (phiNode->size() == 2) {
        const auto value1 = phiNode->getOperand<ir::Value>(0);
        const auto value2 = phiNode->getOperand<ir::Value>(1);
        if (value1 == phiNode) {
          replaceValue = value2;
        } else if (value2 == phiNode) {
          replaceValue = value1;
        }
      }
      if (replaceValue) {
        toContinue = true;
        for (const auto use : replaceValue->getUses()) {
          use->setValue(replaceValue);
          const auto replacePhiNode =
              dynamic_cast<ir::PHINode *>(replaceValue);
          if (replacePhiNode && marked.find(replacePhiNode) == marked.end()) {
            workList.push(replacePhiNode);
            marked.insert(replacePhiNode);
          }
        }
      }
    }
  } while (toContinue);
}

bool PromotePass::runOnFunction(ir::Function *function) {
  if (function->isDeclare()) {
    return false;
  }
  auto promoteAllocaInsts = analyzePromoteAllocaInsts(function);
  if (promoteAllocaInsts.empty()) {
    return false;
  }
  _globalPhiMap.clear();
  for (const auto block : *function) {
    _globalPhiMap.try_emplace(block);
  }
  DomTree domTree(function);
  insertPhi(function, domTree.getDomFrontire(), promoteAllocaInsts);
  std::unordered_map<ir::AllocaInst *, std::stack<ir::Value *>> replaceMap;
  rename(function->getFirst(), replaceMap, promoteAllocaInsts,
         domTree.getDomTree());
  prunePhi(function);
  for (const auto &phiEntry : _globalPhiMap) {
    for (const auto &p : phiEntry.second) {
      const auto phiNode = p.second;
      const auto block = phiEntry.first;
      block->add(0, phiNode);
    }
  }
  for (const auto block : *function) {
    for (int i = 0; i < block->size(); i++) {
      ir::Instruction *inst = block->get(i);
      const auto allocaInst = dynamic_cast<ir::AllocaInst *>(inst);
      if (allocaInst && isAllocaPromotable(allocaInst)) {
        block->remove(i);
        i--;
      }
    }
  }
  return true;
}

} // namespace pass
