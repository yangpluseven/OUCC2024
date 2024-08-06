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
  std::vector<Instruction *> &getInst() { return _instructions; }

  bool isEmpty() const;
  Instruction *getLast() const { return _instructions.back(); }
  Instruction *getFirst() const { return _instructions.back(); }
  void push(Instruction *inst);
  void insert(int index, Instruction *inst);
  void insert(int index, const std::vector<Instruction *> &newInsts);
  void insert(const Instruction *base, Instruction *inst);
  void insert(const Instruction *base,
              const std::vector<Instruction *> &newInst);
  Instruction *erase(int index);
  Instruction *erase(Instruction *inst);
  Instruction *pop();
  Instruction *get(int index) const;
  size_t size() const;

  std::string getName() const override;
  std::string str() const;

  using iterator = std::vector<Instruction *>::iterator;
  using const_iterator = std::vector<Instruction *>::const_iterator;

  iterator begin();
  iterator end();
  const_iterator begin() const;
  const_iterator end() const;
  const_iterator cbegin() const;
  const_iterator cend() const;
};

} // namespace ir

#endif // IR_BASIC_BLOCK_HPP
