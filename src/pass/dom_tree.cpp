#include "dom_tree.hpp"

namespace pass {

void DomTree::analyzePred() {
  if (!_predMap.empty()) {
    return;
  }
  for (const auto block : *_function) {
    _predMap.try_emplace(block);
  }
  for (const auto block : *_function) {
    for (const auto inst : *block) {
      if (const auto branchInst = dynamic_cast<ir::BranchInst *>(inst)) {
        if (branchInst->isConditional()) {
          const auto trueBlock = branchInst->getOperand<ir::BasicBlock>(1);
          const auto falseBlock = branchInst->getOperand<ir::BasicBlock>(2);
          _predMap.at(trueBlock).insert(block);
          _predMap.at(falseBlock).insert(block);
        } else {
          const auto destBlock = branchInst->getOperand<ir::BasicBlock>(0);
          _predMap.at(destBlock).insert(block);
        }
      }
    }
  }
}

void DomTree::anaylzeDom() {
  if (!_domMap.empty()) {
    return;
  }
  for (const auto block : *_function) {
    _domMap.try_emplace(block);
    for (const auto b : *_function) {
      _domMap.at(block).insert(b);
    }
  }
  bool changed;
  do {
    changed = false;
    for (const auto block : *_function) {
      std::unordered_set<ir::BasicBlock *> interSet;
      bool first = true;
      for (const auto b : _predMap.at(block)) {
        const auto &domSet = _domMap.at(b);
        if (first) {
          interSet = domSet;
          first = false;
          continue;
        }
        std::unordered_set<ir::BasicBlock *> tmpSet;
        for (const auto elem : interSet) {
          if (domSet.find(elem) != domSet.end()) {
            tmpSet.insert(elem);
          }
        }
        interSet = std::move(tmpSet);
      }
      interSet.insert(block);
      if (interSet != _domMap.at(block)) {
        changed = true;
        _domMap.at(block) = std::move(interSet);
      }
    }
  } while (changed);
}

void DomTree::analyzeImmDom() {
  if (!_immDomMap.empty()) {
    return;
  }
  std::unordered_map<ir::BasicBlock *, std::unordered_set<ir::BasicBlock *>>
      remainDom;
  std::unordered_set<ir::BasicBlock *> toRemoveBlocks;
  for (const auto &entry : _domMap) {
    const auto block = entry.first;
    auto domBlocks = entry.second;
    domBlocks.erase(block);
    switch (domBlocks.size()) {
    case 0:
      _immDomMap[block] = nullptr;
      break;
    case 1: {
      const auto domBlock = *(domBlocks.begin());
      _immDomMap[block] = domBlock;
      toRemoveBlocks.insert(domBlock);
      break;
    }
    default:
      remainDom.try_emplace(block, domBlocks);
    }
  }
  while (!remainDom.empty()) {
    std::unordered_set<ir::BasicBlock *> nextToRemoveBlocks;
    auto it = remainDom.begin();
    while (it != remainDom.end()) {
      auto &entry = *it;
      const auto block = entry.first;
      auto &domBlocks = entry.second;
      for (const auto b : toRemoveBlocks) {
        if (domBlocks.find(b) != domBlocks.end()) {
          domBlocks.erase(b);
        }
      }
      if (domBlocks.size() == 1) {
        const auto domBlock = *(domBlocks.begin());
        _immDomMap[block] = domBlock;
        nextToRemoveBlocks.insert(domBlock);
        it = remainDom.erase(it);
      } else {
        ++it;
      }
    }
    toRemoveBlocks = std::move(nextToRemoveBlocks);
  }
}

void DomTree::analyzeDomTree() {
  if (!_domTree.empty()) {
    return;
  }
  for (const auto &entry : _immDomMap) {
    const auto from = entry.second;
    const auto to = entry.first;
    if (from) {
      _domTree[from].insert(to);
    }
  }
}

void DomTree::analyzeDomFrontier() {
  if (!_domFrontire.empty()) {
    return;
  }
  for (const auto block : *_function) {
    _domFrontire.try_emplace(block);
  }
  for (const auto block : *_function) {
    if (_predMap.at(block).size() >= 2) {
      for (const auto prevBlock : _predMap.at(block)) {
        ir::BasicBlock *runner = prevBlock;
        while (runner != _immDomMap.at(block)) {
          _domFrontire.at(runner).insert(block);
          runner = _immDomMap.at(runner);
        }
      }
    }
  }
}

} // namespace pass