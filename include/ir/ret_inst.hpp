#ifndef RET_INST_HPP
#define RET_INST_HPP
#include "basic_type.hpp"
#include "instruction.hpp"

namespace ir {

class RetInst : public Instruction {
public:
  RetInst(BasicBlock *block) : Instruction(block, BasicType::VOID) {}

  RetInst(BasicBlock *block, Value *retValue) : Instruction(block, BasicType::VOID, {retValue}) {}

  std::string toString() {
    if (isEmpty())
      return "ret void";
    auto retValue = getOperand<Value>(0);
    return "ret " + retValue->getType()->toString() + " " + retValue->getName();
  }
};

} // namespace ir

#endif // RET_INST_HPP