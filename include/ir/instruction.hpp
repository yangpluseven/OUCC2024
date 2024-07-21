#ifndef IR_INSTRUCTION_HPP
#define IR_INSTRUCTION_HPP
#include "basic_block.hpp"
#include "global_variable.hpp"
#include "value.hpp"
#include <string>

namespace ir {

class Instruction : public User {
private:
  static int _counter;
  BasicBlock *_block;

protected:
  int id;

  Instruction(BasicBlock *block, Type *type,
              const std::initializer_list<Value *> &operands = {})
      : User(type), _block(block) {
    for (auto operand : operands) {
      add(new Use(this, operand));
    }
    id = _counter++;
  }

public:
  BasicBlock *getBlock() { return _block; }

  std::string getName() const override { return "%v" + std::to_string(id); }
};

int Instruction::_counter = 0;

class AllocaInst : public Instruction {
public:
  AllocaInst(BasicBlock *block, Type *type)
      : Instruction(block, new PointerType(type)) {}

  std::string toString() const {
    return getName() + " = alloca " + type->baseType()->toString();
  }
};

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

class GetElementPtrInst : public Instruction {
private:
  static Type *_calcType(Value *value, int indexSize) {
    auto type = value->getType();
    if (dynamic_cast<GlobalVariable *>(type))
      type = new PointerType(type);

    for (int i = 0; i < indexSize; i++)
      type = type->baseType();
    return new PointerType(type);
  }

public:
  GetElementPtrInst(BasicBlock *block, Value *ptr,
                    std::initializer_list<Value *> indexes = {})
      : Instruction(block, _calcType(ptr, indexes.size()), {ptr}) {
    for (auto index : indexes)
      add(new Use(this, index));
  }

  std::string toString() {
    auto ptr = getOperand<Value>(0);
    auto tempPtr = new PointerType(ptr->getType());
    std::stringstream ss;
    ss << getName() << " = getelementptr "
       << (dynamic_cast<GlobalVariable *>(ptr)
               ? ptr->getType()->toString()
               : ptr->getType()->baseType()->toString())
       << ", "
       << (dynamic_cast<GlobalVariable *>(ptr) ? tempPtr->toString()
                                               : ptr->getType()->toString())
       << " " << ptr->getName();
    delete tempPtr;
    for (int i = 1; i < size(); i++) {
      auto operand = getOperand<Value>(i);
      ss << ", " << operand->getType()->toString() << " " << operand->getName();
    }

    return ss.str();
  }
};

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

class PHINode : public Instruction {
private:
  std::unordered_map<Use *, BasicBlock *> useBlockMap;

public:
  PHINode(BasicBlock *block, Type *type) : Instruction(block, type) {}

  void add(BasicBlock *block, Use *use) {
    User::add(use);
    useBlockMap[use] = block;
  }

  std::pair<BasicBlock *, Value *> getBlockValue(int index) {
    auto use = get(index);
    if (useBlockMap.find(use) != useBlockMap.end())
      return {useBlockMap[use], use->getValue()};
    else
      throw "Failed to get the use from this index.";
  }

  void setBlockValue(int index, BasicBlock *block) {
    auto use = get(index);
    if (use != nullptr)
      useBlockMap[use] = block;
    else
      throw "Failed to get the use from this index.";
  }

  std::string toString() {
    std::stringstream ss;
    for (int i = 0; i < operands.size(); i++) {
      if (i != 0)
        ss << ", ";

      ss << "[" << operands[i]->getValue()->getName() << ", "
         << useBlockMap[operands[i]]->toString() << "]";
    }
    return getName() + " = phi " + type->toString() + " " + ss.str();
  }
};

class RetInst : public Instruction {
public:
  RetInst(BasicBlock *block) : Instruction(block, BasicType::VOID) {}

  RetInst(BasicBlock *block, Value *retValue)
      : Instruction(block, BasicType::VOID, {retValue}) {}

  std::string toString() {
    if (isEmpty())
      return "ret void";
    auto retValue = getOperand<Value>(0);
    return "ret " + retValue->getType()->toString() + " " + retValue->getName();
  }
};

class StoreInst : public Instruction {
public:
  StoreInst(BasicBlock *block, Value *value, Value *pointer)
      : Instruction(block, BasicType::VOID, {value, pointer}) {}

  std::string toString() {
    auto value = getOperand<Value>(0);
    auto pointer = getOperand<Value>(1);
    if (auto globalVar = dynamic_cast<GlobalVariable *>(pointer)) {
      return "store " + value->getType()->toString() + value->getName() + ", " +
             globalVar->getType()->toString() + pointer->getName();
    }
    return "store " + value->getType()->toString() + value->getName() + ", " +
           pointer->getType()->toString() + pointer->getName();
  }
};

class BinaryOperator : public Instruction {

public:
  enum Op { ADD, FADD, SUB, FSUB, MUL, FMUL, SDIV, FDIV, SREM, XOR };
  const Op op;
  BinaryOperator(BasicBlock *block, Op op, Value *lhs, Value *rhs)
      : Instruction(block, Type::CheckEquality(lhs->getType(), rhs->getType()),
                    {lhs, rhs}),
        op(op) {}

  std::string toString() const {
    auto lhs = getOperand<Value>(0);
    auto rhs = getOperand<Value>(1);
    auto type = Type::CheckEquality(lhs->getType(), rhs->getType());
    return getName() + " = " + _opToString(op) + " " + type->toString() + " " +
           lhs->getName() + ", " + rhs->getName();
  }

private:
  std::string _opToString(Op v) const noexcept {
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

class CmpInst : public Instruction {
public:
  enum Cond { EQ, NE, SGT, SGE, SLT, SLE, OEQ, OGT, OGE, OLT, OLE, UNE };
  CmpInst(BasicBlock *block, Cond cond, Value *lhs, Value *rhs)
      : Instruction(block, BasicType::I1), cond(cond) {
    add(new Use(this, lhs));
    add(new Use(this, rhs));
  }

  Cond getCond() { return cond; }

  std::string toString() const {
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

class CastInst : public Instruction {
protected:
  CastInst(BasicBlock *block, Type *type, Value *operand)
      : Instruction(block, type, {operand}) {
    add(new Use(this, operand));
  }

  std::string toString() const {
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

#endif // INSTRUCTION_HPP