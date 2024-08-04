#ifndef PASS_DOM_TREE_HPP
#define PASS_DOM_TREE_HPP
#include "function.hpp"

namespace pass {

class DomTree {
private:
  ir::Function *_function;
  std::unordered_map<ir::BasicBlock *, std::unordered_set<ir::BasicBlock *>>
      _predMap;
  std::unordered_map<ir::BasicBlock *, std::unordered_set<ir::BasicBlock *>>
      _domMap;
  std::unordered_map<ir::BasicBlock *, ir::BasicBlock *> _immDomMap;
  std::unordered_map<ir::BasicBlock *, std::unordered_set<ir::BasicBlock *>>
      _domTree;
  std::unordered_map<ir::BasicBlock *, std::unordered_set<ir::BasicBlock *>>
      _domFrontire;

  void analyzePred();
  void anaylzeDom();
  void analyzeImmDom();
  void analyzeDomTree();
  void analyzeDomFrontier();

public:
  explicit DomTree(ir::Function *function) : _function(function) {}

  std::unordered_map<ir::BasicBlock *, ir::BasicBlock *> &getImmdom() {
    analyzePred();
    anaylzeDom();
    analyzeImmDom();
    return _immDomMap;
  }

  std::unordered_map<ir::BasicBlock *, std::unordered_set<ir::BasicBlock *>> &
  getDomTree() {
    analyzePred();
    anaylzeDom();
    analyzeImmDom();
    analyzeDomTree();
    return _domTree;
  }
  std::unordered_map<ir::BasicBlock *, std::unordered_set<ir::BasicBlock *>> &
  getDomFrontire() {
    analyzePred();
    anaylzeDom();
    analyzeImmDom();
    analyzeDomFrontier();
    return _domFrontire;
  }
};

} // namespace pass

#endif // PASS_DOM_TREE_HPP
