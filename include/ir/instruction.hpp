#ifndef INSTRUCTION_HPP
#define INSTRUCTION_HPP
#include "user.hpp"
#include "basic_block.hpp"
#include <string>

namespace ir {

class Instruction : public User {
private:
  static int _counter;
  BasicBlock *_block;

protected:
  int id;

  Instruction(BasicBlock *block, Type *type, std::vector<Value *> &operands)
      : User(type), _block(block) {
    for (auto operand : operands) {
      add(new Use(this, operand));
    }
    id = _counter++;
  }

public:
  BasicBlock *getBlock() const { return _block; }

  std::string getName() const { return "%v" + std::to_string(id); }
};

int Instruction::_counter = 0;

} // namespace ir

#endif // INSTRUCTION_HPP