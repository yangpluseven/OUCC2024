#ifndef IR_BASIC_BLOCK_HPP
#define IR_BASIC_BLOCK_HPP

#include "function.hpp"
#include "instruction.hpp"
#include "value.hpp"
#include <string>
#include <vector>

namespace ir {

class Instruction;

class BasicBlock : public Value {
private:
  static int _counter;
  int _id;
  Function *_function;
  std::vector<Instruction *> _instructions;

public:
  BasicBlock(Function *func);

  bool isEmpty() const;
  Instruction *getLast() const;
  void add(Instruction *inst);
  void add(int index, Instruction *inst);
  void addAll(int index, const std::vector<Instruction *> &newInsts);
  Instruction *remove(int index);
  Instruction *get(int index) const;
  int size();

  std::string getName() const override;
  std::string toString() const;

  class Iterator {
  private:
    typename std::vector<Instruction *>::iterator it;

  public:
    explicit Iterator(typename std::vector<Instruction *>::iterator it);

    Instruction *operator*() const;
    Iterator &operator++();
    Iterator operator++(int);
    bool operator==(const Iterator &rhs) const;
    bool operator!=(const Iterator &rhs) const;
  };

  Iterator begin();
  Iterator end();

  class ConstIterator {
  private:
    typename std::vector<Instruction *>::const_iterator it;

  public:
    explicit ConstIterator(
        typename std::vector<Instruction *>::const_iterator it);

    const Instruction *operator*() const;
    ConstIterator &operator++();
    ConstIterator operator++(int);
    bool operator==(const ConstIterator &other) const;
    bool operator!=(const ConstIterator &other) const;
  };

  ConstIterator begin() const;
  ConstIterator end() const;
};

} // namespace ir

#endif // IR_BASIC_BLOCK_HPP
