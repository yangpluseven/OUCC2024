#ifndef FUNCTION_HPP
#define FUNCTION_HPP

#include "value.hpp"
#include "basic_block.hpp"
#include <vector>
#include <string>
#include <iterator>
#include <sstream>
#include <algorithm>


namespace ir {

class Function : public Value {
private:
  std::string name;
  std::vector<Argument *> args;
  std::vector<BasicBlock *> blocks;

public:
  Function(Type *type, const std::string &name) : Value(type), name(name) {}

  bool isDeclare() const { return blocks.empty(); }

  Function *addArg(Argument *arg) {
    args.push_back(arg);
    return this;
  }

  bool add(BasicBlock *block) {
    blocks.push_back(block);
    return true;
  }

  void add(int index, BasicBlock *block) {
    blocks.insert(blocks.begin() + index, block);
  }

  bool add(int index, const std::vector<BasicBlock *> &newBlocks) {
    blocks.insert(blocks.begin() + index, newBlocks.begin(), newBlocks.end());
    return true;
  }

  BasicBlock *remove(int index) {
    BasicBlock *removed = blocks[index];
    blocks.erase(blocks.begin() + index);
    return removed;
  }

  int size() const { return blocks.size(); }

  BasicBlock *get(int index) const { return blocks[index]; }

  BasicBlock *getFirst() const { return blocks.front(); }

  void insertAfter(BasicBlock *base, BasicBlock *block) {
    auto it = std::find(blocks.begin(), blocks.end(), base);
    if (it != blocks.end()) {
      blocks.insert(it + 1, block);
    }
  }

  std::vector<BasicBlock *>::iterator begin() { return blocks.begin(); }

  std::vector<BasicBlock *>::iterator end() { return blocks.end(); }

  std::vector<BasicBlock *>::const_iterator begin() const {
    return blocks.begin();
  }

  std::vector<BasicBlock *>::const_iterator end() const { return blocks.end(); }

  std::vector<BasicBlock *>::const_iterator cbegin() const {
    return blocks.cbegin();
  }

  std::vector<BasicBlock *>::const_iterator cend() const {
    return blocks.cend();
  }

  std::string getName() const override { return "@" + name; }

  std::string getRawName() const { return name; }

  std::vector<Argument *> getArgs() const { return args; }

  std::string toString() const {
    bool isDeclare = blocks.empty();
    std::ostringstream builder;
    std::ostringstream joiner;
    joiner << "(";
    for (size_t i = 0; i < args.size(); ++i) {
      if (i > 0)
        joiner << ", ";
      if (isDeclare)
        joiner << args[i]->getType()->toString();
      else
        joiner << args[i]->toString();
    }
    joiner << ")";
    if (isDeclare)
      builder << "declare ";
    else
      builder << "define ";
    builder << type->toString() << " " << getName() << joiner.str();
    if (isDeclare)
      return builder.str() + "\n";
    builder << " {\n";
    for (const auto &block : blocks) {
      builder << block->toString() << ":\n";
      for (const auto &inst : *block) {
        builder << "  " << inst->getName() << "\n";
      }
    }
    builder << "}\n";
    return builder.str();
  }
};

class Argument : public Value {
private:
  const std::string name;

public:
  Argument(Type *type, std::string name) : Value(type), name(name) {}

  std::string getName() const override { return "%"+name; }

  std::string toString() const { return type->toString(); }
};

} // namespace ir

#endif // FUNCTION_HPP