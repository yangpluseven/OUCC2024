#ifndef IR_FUNCTION_HPP
#define IR_FUNCTION_HPP

#include "basic_block.hpp"
#include "value.hpp"
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

namespace ir {

class BasicBlock;

class Argument : public Value {
private:
  const std::string name;

public:
  Argument(Type *type, std::string name);

  std::string getName() const override;
  std::string str() const;
};

class Function : public Value {
private:
  std::string _name;
  std::vector<Argument *> _args;
  std::vector<BasicBlock *> _blocks;
  bool _canInline = false;

public:
  Function(Type *type, std::string name);

  bool canInline() const { return _canInline; }
  bool isDeclare() const;

  Function *pushArg(Argument *arg);

  bool pushBlock(BasicBlock *block);
  void insertBlock(int index, BasicBlock *block);
  void insertBlock(int index, const std::vector<BasicBlock *> &newBlocks);
  void insertBlock(const BasicBlock *base, BasicBlock *block);

  BasicBlock *erase(int index);
  size_t size() const;
  BasicBlock *get(int index) const;
  BasicBlock *getFirst() const;
  BasicBlock *getLast() const;

  void analyzeInline() {
    // TODO
    _canInline = true;
  }

  std::string getName() const override;
  std::string getRawName() const;
  std::vector<Argument *> &getArgs();
  std::string str() const;

  using iterator = std::vector<BasicBlock *>::iterator;
  using const_iterator = std::vector<BasicBlock *>::const_iterator;

  iterator begin();
  iterator end();
  const_iterator begin() const;
  const_iterator end() const;
  const_iterator cbegin() const;
  const_iterator cend() const;
};

} // namespace ir

#endif // IR_FUNCTION_HPP
