#include "pass.hpp"
#include "dom_tree.hpp"

#include <queue>

namespace pass {

std::unordered_set<ir::AllocaInst *>
MemoryProm::analyzePromotableAllocaInsts(ir::Function *function) {
  std::unordered_set<ir::AllocaInst *> allocaInsts;
  for (const auto block : *function) {
    for (const auto inst : *block) {
      if (const auto allocaInst = dynamic_cast<ir::AllocaInst *>(inst)) {
        if (isPromotable(allocaInst)) {
          allocaInsts.insert(allocaInst);
        }
      }
    }
  }
  return std::move(allocaInsts);
}

void MemoryProm::insertPhi(
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

void MemoryProm::replace(
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
        block->erase(i);
        i--;
        replaceMap[allocaInst].push(storeInst->getOperand<ir::Value>(0));
        counter[allocaInst] = counter[allocaInst] + 1;
      }
    } else if (const auto loadInst = dynamic_cast<ir::LoadInst *>(inst)) {
      const auto allocaInst =
          dynamic_cast<ir::AllocaInst *>(loadInst->getOperand<ir::Value>(0));
      if (allocaInst && allocaInsts.find(allocaInst) != allocaInsts.end()) {
        block->erase(i);
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
      replace(nextBlock, replaceMap, allocaInsts, domTree);
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

bool MemoryProm::isPromotable(ir::AllocaInst *allocaInst) {
  // return true;
  const auto type = allocaInst->getType();
  if (const auto ptrType = dynamic_cast<ir::PointerType *>(type)) {
    if (dynamic_cast<ir::BasicType *>(ptrType->baseType())) {
      return true;
    }
    if (dynamic_cast<ir::PointerType *>(ptrType->baseType())) {
      return true;
    }
  }
  return false;
}

void MemoryProm::clearPhi(ir::Function *func) {
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

bool MemoryProm::onFunction(ir::Function *function) {
  if (function->isDeclare()) {
    return false;
  }
  auto promoteAllocaInsts = analyzePromotableAllocaInsts(function);
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
  replace(function->getFirst(), replaceMap, promoteAllocaInsts,
         domTree.getDomTree());
  clearPhi(function);
  for (const auto &phiEntry : _globalPhiMap) {
    for (const auto &p : phiEntry.second) {
      const auto phiNode = p.second;
      const auto block = phiEntry.first;
      block->insert(0, phiNode);
    }
  }
  for (const auto block : *function) {
    for (int i = 0; i < block->size(); i++) {
      ir::Instruction *inst = block->get(i);
      const auto allocaInst = dynamic_cast<ir::AllocaInst *>(inst);
      if (allocaInst && isPromotable(allocaInst)) {
        block->erase(i);
        i--;
      }
    }
  }
  return true;
}

} // namespace pass
