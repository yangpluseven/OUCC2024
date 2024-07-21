#ifndef IR_CALL_INST_HPP
#define IR_CALL_INST_HPP

#include "basic_type.hpp"
#include "instruction.hpp"
#include <string>

namespace ir {
class CallInst : public Instruction {
public:
  CallInst(BasicBlock *block, Function *func,
           std::initializer_list<Value *> params)
      : Instruction(block, func->getType(), {func}) {
    for (auto param : params)
      add(new Use(this, param));
  }

  std::string toString() const {
    std::stringstream ss;
    ss << "(";
    for (int i = 1; i < operands.size(); i++) {
      if (i != 1)
        ss << ", ";
      auto param = getOperand<Value>(i);
      ss << param->getType()->toString() << " " << param->getName();
    }
    ss << ")";

    auto func = getOperand<Function>(0);
    if (func->getType() == BasicType::VOID)
      return "call " + func->getType()->toString() + " " + func->getName() +
             ss.str();
    return getName() + " = call " + func->getType()->toString() + " " +
           func->getName() + ss.str();
  }
};
} // namespace ir

#endif