#ifndef PASS_DOM_TREE_HPP
#define PASS_DOM_TREE_HPP
#include "function.hpp"

namespace pass {

class DomTree {
private:
  ir::Function *_function;
  std::unordered_map<ir::BasicBlock *, std::unordered_set<ir::BasicBlock *>>
      _prevMap;
  std::unordered_map<ir::BasicBlock *, std::unordered_set<ir::BasicBlock *>>
      _domMap;
  std::unordered_map<ir::BasicBlock *, ir::BasicBlock *> _idomMap;
  std::unordered_map<ir::BasicBlock *, std::unordered_set<ir::BasicBlock *>>
      _domTree;
  std::unordered_map<ir::BasicBlock *, std::unordered_set<ir::BasicBlock *>>
      _domFrontire;

  void analyzePrev();
  void analyzeDom();
  void analyzeIdom();
  void analyzeDomTree();
  void analyzeDF();

public:
  explicit DomTree(ir::Function *function) : _function(function) {}

  std::unordered_map<ir::BasicBlock *, ir::BasicBlock *> &getIdom() {
    analyzePrev();
    analyzeDom();
    analyzeIdom();
    return _idomMap;
  }

  std::unordered_map<ir::BasicBlock *, std::unordered_set<ir::BasicBlock *>> &
  getDomTree() {
    analyzePrev();
    analyzeDom();
    analyzeIdom();
    analyzeDomTree();
    return _domTree;
  }
  std::unordered_map<ir::BasicBlock *, std::unordered_set<ir::BasicBlock *>> &
  getDomFrontire() {
    analyzePrev();
    analyzeDom();
    analyzeIdom();
    analyzeDF();
    return _domFrontire;
  }
};

} // namespace pass

#endif // PASS_DOM_TREE_HPP
