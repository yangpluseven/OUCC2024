#include "ir/instruction.hpp"
#include "ir/global_variable.hpp"
#include <stdexcept>

namespace ir {

// Static member initialization
int Instruction::_counter = 0;

Instruction::Instruction(BasicBlock *block, Type *type,
                         const std::initializer_list<Value *> &operands)
    : User(type), _block(block) {
  for (auto operand : operands) {
    add(new Use(this, operand));
  }
  id = _counter++;
}

Instruction::Instruction(BasicBlock *block, Type *type,
                         std::vector<Value *> &operands)
    : User(type), _block(block) {
  for (auto operand : operands) {
    add(new Use(this, operand));
  }
  id = _counter++;
}

BasicBlock *Instruction::getBlock() { return _block; }

std::string Instruction::getName() const { return "%v" + std::to_string(id); }

AllocaInst::AllocaInst(BasicBlock *block, Type *type)
    : Instruction(block, new PointerType(type)) {}

std::string AllocaInst::toString() const {
  return getName() + " = alloca " + type->baseType()->toString();
}

BranchInst::BranchInst(BasicBlock *block, BasicBlock *dest)
    : Instruction(block, BasicType::VOID, {dest}) {}

BranchInst::BranchInst(BasicBlock *block, Value *cond, BasicBlock *ifTrue,
                       BasicBlock *ifFalse)
    : Instruction(block, BasicType::VOID, {cond, ifTrue, ifFalse}) {}

bool BranchInst::isConditional() const { return size() == 3; }

std::string BranchInst::toString() const {
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

CallInst::CallInst(BasicBlock *block, Function *func,
                   std::initializer_list<Value *> params)
    : Instruction(block, func->getType(), {func}) {
  for (auto param : params)
    add(new Use(this, param));
}

CallInst::CallInst(BasicBlock *block, Function *func,
                   std::vector<Value *> &params)
    : Instruction(block, func->getType(), {func}) {
  for (auto param : params)
    add(new Use(this, param));
}

std::string CallInst::toString() const {
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

Type *GetElementPtrInst::_calcType(Value *value, int indexSize) {
  auto type = value->getType();
  if (dynamic_cast<GlobalVariable *>(type))
    type = new PointerType(type);

  for (int i = 0; i < indexSize; i++)
    type = type->baseType();
  return new PointerType(type);
}

GetElementPtrInst::GetElementPtrInst(BasicBlock *block, Value *ptr,
                                     std::initializer_list<Value *> indexes)
    : Instruction(block, _calcType(ptr, indexes.size()), {ptr}) {
  for (auto index : indexes)
    add(new Use(this, index));
}

GetElementPtrInst::GetElementPtrInst(BasicBlock *block, Value *ptr,
                                     std::vector<Value *> &indexes)
    : Instruction(block, _calcType(ptr, indexes.size()), {ptr}) {
  for (auto index : indexes)
    add(new Use(this, index));
}

std::string GetElementPtrInst::toString() {
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

LoadInst::LoadInst(BasicBlock *block, Value *ptr)
    : Instruction(block,
                  dynamic_cast<GlobalVariable *>(ptr)
                      ? ptr->getType()
                      : ptr->getType()->baseType(),
                  {ptr}) {}

std::string LoadInst::toString() {
  auto ptr = getOperand<Value>(0);
  return getName() + " = load " + type->toString() + ", " +
         (dynamic_cast<GlobalVariable *>(ptr)
              ? (new PointerType(ptr->getType()))->toString()
              : ptr->getType()->toString()) +
         " " + ptr->getName();
}

PHINode::PHINode(BasicBlock *block, Type *type) : Instruction(block, type) {}

void PHINode::add(BasicBlock *block, Use *use) {
  User::add(use);
  useBlockMap[use] = block;
}

std::pair<BasicBlock *, Value *> PHINode::getBlockValue(int index) {
  auto use = get(index);
  if (useBlockMap.find(use) != useBlockMap.end())
    return {useBlockMap[use], use->getValue()};
  else
    throw std::runtime_error("Failed to get the use from this index.");
}

void PHINode::setBlockValue(int index, BasicBlock *block) {
  auto use = get(index);
  if (use != nullptr)
    useBlockMap[use] = block;
  else
    throw std::runtime_error("Failed to get the use from this index.");
}

std::string PHINode::toString() {
  std::stringstream ss;
  for (int i = 0; i < operands.size(); i++) {
    if (i != 0)
      ss << ", ";

    ss << "[" << operands[i]->getValue()->getName() << ", "
       << useBlockMap[operands[i]]->toString() << "]";
  }
  return getName() + " = phi " + type->toString() + " " + ss.str();
}

RetInst::RetInst(BasicBlock *block) : Instruction(block, BasicType::VOID) {}

RetInst::RetInst(BasicBlock *block, Value *retValue)
    : Instruction(block, BasicType::VOID, {retValue}) {}

std::string RetInst::toString() {
  if (isEmpty())
    return "ret void";
  auto retValue = getOperand<Value>(0);
  return "ret " + retValue->getType()->toString() + " " + retValue->getName();
}

StoreInst::StoreInst(BasicBlock *block, Value *value, Value *pointer)
    : Instruction(block, BasicType::VOID, {value, pointer}) {}

std::string StoreInst::toString() {
  auto value = getOperand<Value>(0);
  auto pointer = getOperand<Value>(1);
  if (auto globalVar = dynamic_cast<GlobalVariable *>(pointer)) {
    return "store " + value->getType()->toString() + value->getName() + ", " +
           globalVar->getType()->toString() + pointer->getName();
  }
  return "store " + value->getType()->toString() + value->getName() + ", " +
         pointer->getType()->toString() + pointer->getName();
}

BinaryOperator::BinaryOperator(BasicBlock *block, Op op, Value *lhs, Value *rhs)
    : Instruction(block, Type::CheckEquality(lhs->getType(), rhs->getType())),
      op(op) {}

std::string BinaryOperator::toString() const {
  auto lhs = getOperand<Value>(0);
  auto rhs = getOperand<Value>(1);
  auto type = Type::CheckEquality(lhs->getType(), rhs->getType());
  return getName() + " = " + _opToString(op) + " " + type->toString() + " " +
         lhs->getName() + ", " + rhs->getName();
}

std::string BinaryOperator::_opToString(Op v) const noexcept {
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
    throw std::runtime_error("OutOfRangeException");
  };
}

CmpInst::CmpInst(BasicBlock *block, Cond cond, Value *lhs, Value *rhs)
    : Instruction(block, BasicType::I1), cond(cond) {
  add(new Use(this, lhs));
  add(new Use(this, rhs));
}

CmpInst::Cond CmpInst::getCond() const { return cond; }

std::string CmpInst::toString(const std::string &class_name) const {
  auto cond = getCond();
  auto op1 = getOperand<Value>(0);
  auto op2 = getOperand<Value>(1);
  auto type = Type::CheckEquality(op1->getType(), op2->getType());
  return getName() + " = " + class_name + " " + _condToString(cond) + " " +
         type->toString() + " " + op1->getName() + ", " + op2->getName();
}

std::string CmpInst::_condToString(Cond v) const {
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
    throw std::runtime_error("OutOfRangeException");
  };
}

FCmpInst::FCmpInst(BasicBlock *block, Cond cond, Value *lhs, Value *rhs)
    : CmpInst(block, cond, lhs, rhs) {}

std::string FCmpInst::toString() const { return CmpInst::toString("fcmp"); }

ICmpInst::ICmpInst(BasicBlock *block, Cond cond, Value *lhs, Value *rhs)
    : CmpInst(block, cond, lhs, rhs) {}

std::string ICmpInst::toString() const { return CmpInst::toString("icmp"); }

CastInst::CastInst(BasicBlock *block, Type *type, Value *operand)
    : Instruction(block, type, {operand}) {
  add(new Use(this, operand));
}

std::string CastInst::toString(const std::string &class_name) const {
  auto operand = getOperand<Value>(0);
  return getName() + " = " + class_name + " " + operand->getType()->toString() +
         " " + operand->getName() + " to " + type->toString();
}

BitCastInst::BitCastInst(BasicBlock *block, Type *type, Value *operand)
    : CastInst(block, type, operand) {}

std::string BitCastInst::toString() const {
  return CastInst::toString("bitcast");
}

FPToSIInst::FPToSIInst(BasicBlock *block, Type *type, Value *operand)
    : CastInst(block, type, operand) {}

std::string FPToSIInst::toString() const {
  return CastInst::toString("fptosi");
}

SExtInst::SExtInst(BasicBlock *block, Type *type, Value *operand)
    : CastInst(block, type, operand) {}

std::string SExtInst::toString() const { return CastInst::toString("sext"); }

SIToFPInst::SIToFPInst(BasicBlock *block, Type *type, Value *operand)
    : CastInst(block, type, operand) {}

std::string SIToFPInst::toString() const {
  return CastInst::toString("sitofp");
}

ZExtInst::ZExtInst(BasicBlock *block, Type *type, Value *operand)
    : CastInst(block, type, operand) {}

std::string ZExtInst::toString() const { return CastInst::toString("zext"); }

} // namespace ir
