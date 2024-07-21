#ifndef IR_BASIC_BLOCK_HPP
#define IR_BASIC_BLOCK_HPP
#include "basic_type.hpp"
#include "function.hpp"
#include "instruction.hpp"
#include "value.hpp"
#include <stdexcept>

namespace ir {

class BasicBlock : public Value {
private:
  static int _counter;
  int _id;
  Function *_function;
  std::vector<Instruction *> _instructions;

public:
  BasicBlock(Function *func)
      : Value(BasicType::VOID), _function(func), _id(_counter++) {}

  bool isEmpty() const { return _instructions.empty(); }

  Instruction *getLast() const {
    if (!_instructions.empty()) {
      return _instructions.back();
    } else {
      throw std::out_of_range("No _instructions in the BasicBlock");
    }
  }

  void add(Instruction *inst) { _instructions.push_back(inst); }

  void add(int index, Instruction *inst) {
    if (index < 0 || index > _instructions.size()) {
      throw std::out_of_range("Index out of range");
    }
    _instructions.insert(_instructions.begin() + index, inst);
  }

  void addAll(int index, const std::vector<Instruction *> &newInsts) {
    if (index < 0 || index > _instructions.size()) {
      throw std::out_of_range("Index out of range");
    }
    _instructions.insert(_instructions.begin() + index, newInsts.begin(),
                         newInsts.end());
  }

  Instruction *remove(int index) {
    if (index < 0 || index >= _instructions.size()) {
      throw std::out_of_range("Index out of range");
    }
    Instruction *removedInst = _instructions[index];
    _instructions.erase(_instructions.begin() + index);
    return removedInst;
  }

  Instruction *get(int index) const {
    if (index < 0 || index >= _instructions.size()) {
      throw std::out_of_range("Index out of range");
    }
    return _instructions[index];
  }

  int size() { return _instructions.size(); }

  std::string getName() const override { return "b" + std::to_string(_id); }

  std::string toString() const { return getName(); }

  std::vector<Instruction *>::iterator begin() { return _instructions.begin(); }

  std::vector<Instruction *>::iterator end() { return _instructions.end(); }

  std::vector<Instruction *>::const_iterator begin() const {
    return _instructions.begin();
  }

  std::vector<Instruction *>::const_iterator end() const {
    return _instructions.end();
  }
};

int BasicBlock::_counter = 0;

} // namespace ir

#endif