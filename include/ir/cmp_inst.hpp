#ifndef IR_CMP_INST_HPP
#define IR_CMP_INST_HPP

#include "basic_type.hpp"
#include "instruction.hpp"

namespace ir {
class CmpInst : public Instruction {
public:
  enum Cond { EQ, NE, SGT, SGE, SLT, SLE, OEQ, OGT, OGE, OLT, OLE, UNE };
  CmpInst(BasicBlock *block, Cond cond, Value *lhs, Value *rhs)
      : Instruction(block, BasicType::I1), cond(cond) {
    add(new Use(this, lhs));
    add(new Use(this, rhs));
  }

  Cond getCond() { return cond; }

  std::string toString() {
    // TODO: Implement this
  }

private:
  Cond cond;

  std::string _condToString(Cond v) {
    switch (v) {
    case EQ:
      return "eq";
    case NE:
      return "ne";
    case SGT:
      return "sgt";
    case SGE:
      return "sge";
    case SLT:
      return "slt";
    case SLE:
      return "sle";
    case OEQ:
      return "oeq";
    case OGT:
      return "ogt";
    case OGE:
      return "oge";
    case OLT:
      return "olt";
    case OLE:
      return "ole";
    case UNE:
      return "une";
    default:
      throw "OutOfRangeException";
    };
  }
};

class FCmpInst : public CmpInst {
public:
  FCmpInst(BasicBlock *block, Cond cond, Value *lhs, Value *rhs)
      : CmpInst(block, cond, lhs, rhs) {}
};

class ICmpInst : public CmpInst {
public:
  ICmpInst(BasicBlock *block, Cond cond, Value *lhs, Value *rhs)
      : CmpInst(block, cond, lhs, rhs) {}
};

} // namespace ir

#endif