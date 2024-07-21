#ifndef IR_BRANCH_INST_HPP
#define IR_BRANCH_INST_HPP

#include "basic_type.hpp"
#include "instruction.hpp"
namespace ir {
class BranchInst : public Instruction {
public:
  BranchInst(BasicBlock *block, BasicBlock *dest)
      : Instruction(block, BasicType::VOID, {dest}) {}

  BranchInst(BasicBlock *block, Value *cond, BasicBlock *ifTrue,
             BasicBlock *ifFalse)
      : Instruction(block, BasicType::VOID, {cond, ifTrue, ifFalse}) {}

  bool isConditional() const { return size() == 3; }

  std::string toString() const {
    if (isConditional()) {
      auto cond = getOperand<Value>(0);
      auto ifTrue = getOperand<BasicBlock>(1);
      auto ifFalse = getOperand<BasicBlock>(2);
      return "br i1 " + cond->getName() + ", label " + ifTrue->toString() +
             ", label " + ifFalse->toString();
    }
    auto dest = getOperand<BasicBlock>(0);
    return "br label " + dest->toString();
  }
};
} // namespace ir

#endif