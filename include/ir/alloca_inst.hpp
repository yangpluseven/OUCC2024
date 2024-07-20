#ifndef IR_ALLOCA_INST_HPP
#define IR_ALLOCA_INST_HPP

#include "instruction.hpp"
#include "pointer_type.hpp"

namespace ir {
class AllocaInst : public Instruction {
public:
  AllocaInst(BasicBlock *block, Type *type)
      : Instruction(block, new PointerType(type)) {}

  std::string toString() {
    return getName() + " = alloca " + type->baseType()->toString();
  }
};
} // namespace ir

#endif