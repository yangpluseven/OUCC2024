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
  explicit BasicBlock(Function *func);

  bool isEmpty() const;
  Instruction *getLast() const;
  void add(Instruction *inst);
  void add(int index, Instruction *inst);
  void addAll(int index, const std::vector<Instruction *> &newInsts);
  Instruction *remove(int index);
  Instruction *get(int index) const;
  size_t size() const;

  std::string getName() const override;
  std::string toString() const;

  std::vector<Instruction *>::iterator begin();
  std::vector<Instruction *>::iterator end();
  std::vector<Instruction *>::const_iterator begin() const;
  std::vector<Instruction *>::const_iterator end() const;
  std::vector<Instruction *>::const_iterator cbegin() const;
  std::vector<Instruction *>::const_iterator cend() const;
};

} // namespace ir

#endif // IR_BASIC_BLOCK_HPP
