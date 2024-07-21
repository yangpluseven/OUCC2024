#ifndef IR_CAST_INST_HPP
#define IR_CAST_INST_HPP

#include "instruction.hpp"

namespace ir {
class CastInst : public Instruction {
protected:
  CastInst(BasicBlock *block, Type *type, Value *operand)
      : Instruction(block, type, {operand}) {
    add(new Use(this, operand));
  }

  std::string toString() const{
    // TODO: Implement this
  }
};

class BitCastInst : public CastInst {
public:
  BitCastInst(BasicBlock *block, Type *type, Value *operand)
      : CastInst(block, type, operand) {}
};

class FPToSIInst : public CastInst {
public:
  FPToSIInst(BasicBlock *block, Type *type, Value *operand)
      : CastInst(block, type, operand) {}
};

class SExtInst : public CastInst {
public:
  SExtInst(BasicBlock *block, Type *type, Value *operand)
      : CastInst(block, type, operand) {}
};

class SIToFPInst : public CastInst {
public:
  SIToFPInst(BasicBlock *block, Type *type, Value *operand)
      : CastInst(block, type, operand) {}
};

class ZExtInst : public CastInst {
public:
  ZExtInst(BasicBlock *block, Type *type, Value *operand)
      : CastInst(block, type, operand) {}
};
} // namespace ir

#endif