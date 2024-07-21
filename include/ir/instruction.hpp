#ifndef IR_INSTRUCTION_HPP
#define IR_INSTRUCTION_HPP
#include "basic_block.hpp"
#include "user.hpp"
#include <string>

namespace ir {

class Instruction : public User {
private:
  static int _counter;
  BasicBlock *_block;

protected:
  int id;

  Instruction(BasicBlock *block, Type *type,
              const std::initializer_list<Value *> &operands = {})
      : User(type), _block(block) {
    for (auto operand : operands) {
      add(new Use(this, operand));
    }
    id = _counter++;
  }

public:
  BasicBlock *getBlock() { return _block; }

  std::string getName() const override { return "%v" + std::to_string(id); }
};

int Instruction::_counter = 0;

} // namespace ir

#endif // INSTRUCTION_HPP