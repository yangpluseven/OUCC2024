#ifndef IR_Binary_Operator_HPP
#define IR_Binary_Operator_HPP

#include "instruction.hpp"

namespace ir {
class BinaryOperator : public Instruction {

public:
  enum Op { ADD, FADD, SUB, FSUB, MUL, FMUL, SDIV, FDIV, SREM, XOR };
  const Op op;
  BinaryOperator(BasicBlock *block, Op op, Value *lhs, Value *rhs)
      : Instruction(block, Type::CheckEquality(lhs->getType(), rhs->getType()),
                    {lhs, rhs}),
        op(op) {}

  std::string toString() {
    auto lhs = getOperand<Value>(0);
    auto rhs = getOperand<Value>(1);
    auto type = Type::CheckEquality(lhs->getType(), rhs->getType());
    return getName() + " = " + _opToString(op) + " " + type->toString() + " " +
           lhs->getName() + ", " + rhs->getName();
  }

private:
  std::string _opToString(Op v) {
    switch (v) {
    case ADD:
      return "add";
    case FADD:
      return "fadd";
    case SUB:
      return "sub";
    case FSUB:
      return "fsub";
    case MUL:
      return "mul";
    case FMUL:
      return "fmul";
    case SDIV:
      return "sdiv";
    case FDIV:
      return "fdiv";
    case SREM:
      return "srem";
    case XOR:
      return "xor";
    default:
      throw "OutOfRangeException";
    };
  }
};
} // namespace ir
#endif