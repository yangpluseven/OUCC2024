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

public:
  Function(Type *type, std::string name);

  bool isDeclare() const;

  Function *pushArg(Argument *arg);

  bool pushBlock(BasicBlock *block);

  void insertBlock(int index, BasicBlock *block);

  bool insertBlock(int index, const std::vector<BasicBlock *> &newBlocks);

  BasicBlock *erase(int index);

  size_t size() const;

  BasicBlock *get(int index) const;

  BasicBlock *getFirst() const;

  void insertAfter(const BasicBlock *base, BasicBlock *block);

  std::vector<BasicBlock *>::iterator begin();
  std::vector<BasicBlock *>::iterator end();
  std::vector<BasicBlock *>::const_iterator begin() const;
  std::vector<BasicBlock *>::const_iterator end() const;
  std::vector<BasicBlock *>::const_iterator cbegin() const;
  std::vector<BasicBlock *>::const_iterator cend() const;

  std::string getName() const override;
  std::string getRawName() const;
  std::vector<Argument *> &getArgs();
  std::string str() const;
};

} // namespace ir

#endif // IR_FUNCTION_HPP
