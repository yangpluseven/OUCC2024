#ifndef IR_INSTRUCTION_HPP
#define IR_INSTRUCTION_HPP

#include "basic_block.hpp"
#include "value.hpp"
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
  Instruction(BasicBlock *block, Type *type, std::vector<Value *> &operands);

public:
  BasicBlock *getBlock() const;
  void setBlock(BasicBlock *block) { _block = block; }
  std::string getName() const override;
  virtual std::string str() const;
  virtual Instruction *
  clone(std::unordered_map<Value *, Value *> &replaceMap) const = 0;
};

class AllocaInst : public Instruction {
public:
  AllocaInst(BasicBlock *block, Type *type);
  std::string str() const override;
  Instruction *
  clone(std::unordered_map<Value *, Value *> &replaceMap) const override;
};

class BranchInst : public Instruction {
public:
  explicit BranchInst(BasicBlock *block)
      : Instruction(block, BasicType::VOID) {}
  BranchInst(BasicBlock *block, BasicBlock *dest);
  BranchInst(BasicBlock *block, Value *cond, BasicBlock *ifTrue,
             BasicBlock *ifFalse);
  bool isConditional() const;
  std::string str() const override;
  Instruction *
  clone(std::unordered_map<Value *, Value *> &replaceMap) const override;
};

class CallInst : public Instruction {
public:
  explicit CallInst(BasicBlock *block) : Instruction(block, BasicType::VOID) {}
  CallInst(BasicBlock *block, Function *func,
           std::initializer_list<Value *> params);
  CallInst(BasicBlock *block, Function *func, std::vector<Value *> &params);
  std::string str() const override;
  Instruction *
  clone(std::unordered_map<Value *, Value *> &replaceMap) const override;
};

class GetPtrInst : public Instruction {
private:
  static Type *calcType(Value *value, size_t indexSize);

public:
  explicit GetPtrInst(BasicBlock *block, Type *type)
      : Instruction(block, type) {}
  GetPtrInst(BasicBlock *block, Value *ptr,
             std::initializer_list<Value *> indexes = {});
  GetPtrInst(BasicBlock *block, Value *ptr, std::vector<Value *> &indexes);
  std::string str() const override;
  Instruction *
  clone(std::unordered_map<Value *, Value *> &replaceMap) const override;
};

class LoadInst : public Instruction {
public:
  LoadInst(BasicBlock *block, Type *type) : Instruction(block, type) {}
  LoadInst(BasicBlock *block, Value *ptr);
  std::string str() const override;
  Instruction *
  clone(std::unordered_map<Value *, Value *> &replaceMap) const override;
};

class PHINode : public Instruction {
private:
  std::unordered_map<Use *, BasicBlock *> _useBlockMap;

public:
  PHINode(BasicBlock *block, Type *type) : Instruction(block, type) {}
  void add(BasicBlock *block, Use *use);
  std::pair<BasicBlock *, Value *> getBlockValue(int index);
  void setBlockValue(int index, BasicBlock *block);
  std::string str() const override;

  BasicBlock *getUseBlock(Use *use) const { return _useBlockMap.at(use); }
  BasicBlock *getUseBlock(int index) const { return getUseBlock(get(index)); }

  Instruction *
  clone(std::unordered_map<Value *, Value *> &replaceMap) const override;
};

class RetInst : public Instruction {
public:
  explicit RetInst(BasicBlock *block);
  RetInst(BasicBlock *block, Value *retValue);
  std::string str() const override;
  Instruction *
  clone(std::unordered_map<Value *, Value *> &replaceMap) const override;
};

class StoreInst : public Instruction {
public:
  explicit StoreInst(BasicBlock *block) : Instruction(block, BasicType::VOID) {}
  StoreInst(BasicBlock *block, Value *value, Value *pointer);
  std::string str() const override;
  Instruction *
  clone(std::unordered_map<Value *, Value *> &replaceMap) const override;
};

class BinaryInst : public Instruction {
public:
  enum Op { ADD, FADD, SUB, FSUB, MUL, FMUL, SDIV, FDIV, SREM, XOR };

  const Op op;
  BinaryInst(BasicBlock *block, Type *type, Op op)
      : Instruction(block, type), op(op) {}
  BinaryInst(BasicBlock *block, Op op, Value *lhs, Value *rhs);
  std::string str() const override;
  Instruction *
  clone(std::unordered_map<Value *, Value *> &replaceMap) const override;

private:
  static std::string _opToString(Op v) noexcept;
};

class CmpInst : public Instruction {
public:
  enum Cond { EQ, NE, SGT, SGE, SLT, SLE, OEQ, OGT, OGE, OLT, OLE, UNE };

  CmpInst(BasicBlock *block, Cond cond)
      : Instruction(block, BasicType::I1), cond(cond) {}
  CmpInst(BasicBlock *block, Cond cond, Value *lhs, Value *rhs);
  Cond getCond() const;
  std::string str() const override;
  virtual std::string getClassName() const;

private:
  Cond cond;
  static std::string _condToString(Cond v);
};

class FCmpInst : public CmpInst {
public:
  FCmpInst(BasicBlock *block, Cond cond) : CmpInst(block, cond) {}
  FCmpInst(BasicBlock *block, Cond cond, Value *lhs, Value *rhs);
  std::string getClassName() const override;
  Instruction *
  clone(std::unordered_map<Value *, Value *> &replaceMap) const override;
};

class ICmpInst : public CmpInst {
public:
  ICmpInst(BasicBlock *block, Cond cond) : CmpInst(block, cond) {}
  ICmpInst(BasicBlock *block, Cond cond, Value *lhs, Value *rhs);
  std::string getClassName() const override;
  Instruction *
  clone(std::unordered_map<Value *, Value *> &replaceMap) const override;
};

class CastInst : public Instruction {
protected:
  CastInst(BasicBlock *block, Type *type) : Instruction(block, type) {}
  CastInst(BasicBlock *block, Type *type, Value *operand);
  std::string str() const override;
  virtual std::string getClassName() const;
};

class BitCastInst : public CastInst {
public:
  BitCastInst(BasicBlock *block, Type *type) : CastInst(block, type) {}
  BitCastInst(BasicBlock *block, Type *type, Value *operand);
  std::string getClassName() const override;
  Instruction *
  clone(std::unordered_map<Value *, Value *> &replaceMap) const override;
};

class FPToSIInst : public CastInst {
public:
  FPToSIInst(BasicBlock *block, Type *type) : CastInst(block, type) {}
  FPToSIInst(BasicBlock *block, Type *type, Value *operand);
  std::string getClassName() const override;
  Instruction *
  clone(std::unordered_map<Value *, Value *> &replaceMap) const override;
};

class SExtInst : public CastInst {
public:
  SExtInst(BasicBlock *block, Type *type) : CastInst(block, type) {}
  SExtInst(BasicBlock *block, Type *type, Value *operand);
  std::string getClassName() const override;
  Instruction *
  clone(std::unordered_map<Value *, Value *> &replaceMap) const override;
};

class SIToFPInst : public CastInst {
public:
  SIToFPInst(BasicBlock *block, Type *type) : CastInst(block, type) {}
  SIToFPInst(BasicBlock *block, Type *type, Value *operand);
  std::string getClassName() const override;
  Instruction *
  clone(std::unordered_map<Value *, Value *> &replaceMap) const override;
};

class ZExtInst : public CastInst {
public:
  ZExtInst(BasicBlock *block, Type *type) : CastInst(block, type) {}
  ZExtInst(BasicBlock *block, Type *type, Value *operand);
  std::string getClassName() const override;
  Instruction *
  clone(std::unordered_map<Value *, Value *> &replaceMap) const override;
};

} // namespace ir

#endif // IR_INSTRUCTION_HPP