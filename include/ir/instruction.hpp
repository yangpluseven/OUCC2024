#ifndef IR_INSTRUCTION_HPP
#define IR_INSTRUCTION_HPP

#include "basic_block.hpp"
#include "global_variable.hpp"
#include "value.hpp"
#include <sstream>
#include <string>
#include <unordered_map>

namespace ir {

class BasicBlock;
class Function;

class Instruction : public User {
private:
  static int _counter;
  BasicBlock *_block;

protected:
  int id;

  Instruction(BasicBlock *block, Type *type,
              const std::initializer_list<Value *> &operands = {});

public:
  BasicBlock *getBlock();
  std::string getName() const override;
};

class AllocaInst : public Instruction {
public:
  AllocaInst(BasicBlock *block, Type *type);
  std::string toString() const;
};

class BranchInst : public Instruction {
public:
  BranchInst(BasicBlock *block, BasicBlock *dest);
  BranchInst(BasicBlock *block, Value *cond, BasicBlock *ifTrue,
             BasicBlock *ifFalse);
  bool isConditional() const;
  std::string toString() const;
};

class CallInst : public Instruction {
public:
  CallInst(BasicBlock *block, Function *func,
           std::initializer_list<Value *> params);
  std::string toString() const;
};

class GetElementPtrInst : public Instruction {
private:
  static Type *_calcType(Value *value, int indexSize);

public:
  GetElementPtrInst(BasicBlock *block, Value *ptr,
                    std::initializer_list<Value *> indexes = {});
  std::string toString();
};

class LoadInst : public Instruction {
public:
  LoadInst(BasicBlock *block, Value *ptr);
  std::string toString();
};

class PHINode : public Instruction {
private:
  std::unordered_map<Use *, BasicBlock *> useBlockMap;

public:
  PHINode(BasicBlock *block, Type *type);
  void add(BasicBlock *block, Use *use);
  std::pair<BasicBlock *, Value *> getBlockValue(int index);
  void setBlockValue(int index, BasicBlock *block);
  std::string toString();
};

class RetInst : public Instruction {
public:
  RetInst(BasicBlock *block);
  RetInst(BasicBlock *block, Value *retValue);
  std::string toString();
};

class StoreInst : public Instruction {
public:
  StoreInst(BasicBlock *block, Value *value, Value *pointer);
  std::string toString();
};

class BinaryOperator : public Instruction {
public:
  enum Op { ADD, FADD, SUB, FSUB, MUL, FMUL, SDIV, FDIV, SREM, XOR };
  const Op op;
  BinaryOperator(BasicBlock *block, Op op, Value *lhs, Value *rhs);
  std::string toString() const;

private:
  std::string _opToString(Op v) const noexcept;
};

class CmpInst : public Instruction {
public:
  enum Cond { EQ, NE, SGT, SGE, SLT, SLE, OEQ, OGT, OGE, OLT, OLE, UNE };
  CmpInst(BasicBlock *block, Cond cond, Value *lhs, Value *rhs);
  Cond getCond();
  virtual std::string toString() const = 0;

private:
  Cond cond;
  std::string _condToString(Cond v);
};

class FCmpInst : public CmpInst {
public:
  FCmpInst(BasicBlock *block, Cond cond, Value *lhs, Value *rhs);
};

class ICmpInst : public CmpInst {
public:
  ICmpInst(BasicBlock *block, Cond cond, Value *lhs, Value *rhs);
};

class CastInst : public Instruction {
protected:
  CastInst(BasicBlock *block, Type *type, Value *operand);
  virtual std::string toString() const = 0;
};

class BitCastInst : public CastInst {
public:
  BitCastInst(BasicBlock *block, Type *type, Value *operand);
};

class FPToSIInst : public CastInst {
public:
  FPToSIInst(BasicBlock *block, Type *type, Value *operand);
};

class SExtInst : public CastInst {
public:
  SExtInst(BasicBlock *block, Type *type, Value *operand);
};

class SIToFPInst : public CastInst {
public:
  SIToFPInst(BasicBlock *block, Type *type, Value *operand);
};

class ZExtInst : public CastInst {
public:
  ZExtInst(BasicBlock *block, Type *type, Value *operand);
};

} // namespace ir

#endif // IR_INSTRUCTION_HPP
