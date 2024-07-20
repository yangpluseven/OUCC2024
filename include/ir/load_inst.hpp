#ifndef IR_LOAD_INST_HPP
#define IR_LOAD_INST_HPP

#include "global_variable.hpp"
#include "instruction.hpp"
#include "pointer_type.hpp"
#include <string>

namespace ir {
class LoadInst : public Instruction {
public:
  LoadInst(BasicBlock *block, Value *ptr)
      : Instruction(block,
                    dynamic_cast<GlobalVariable *>(ptr)
                        ? ptr->getType()
                        : ptr->getType()->baseType(),
                    {ptr}) {}

  std::string toString() {
    auto ptr = getOperand<Value>(0);
    return getName() + " = load " + type->toString() + ", " +
           (dynamic_cast<GlobalVariable *>(ptr)
                ? (new PointerType(ptr->getType()))->toString()
                : ptr->getType()->toString()) +
           " " + ptr->getName();
  }
};
} // namespace ir

#endif