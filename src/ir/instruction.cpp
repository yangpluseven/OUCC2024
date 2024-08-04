#include "instruction.hpp"
#include "global_variable.hpp"
#include <stdexcept>

namespace ir {

// Static member initialization
int Instruction::_counter = 0;

Instruction::Instruction(BasicBlock *block, Type *type,
                         const std::initializer_list<Value *> &operands)
  : User(type), _block(block) {
  for (auto operand : operands) {
    insert(new Use(this, operand));
  }
  id = _counter++;
}

Instruction::Instruction(BasicBlock *block, Type *type,
                         std::vector<Value *> &operands)
  : User(type), _block(block) {
  for (auto operand : operands) {
    insert(new Use(this, operand));
  }
  id = _counter++;
}

BasicBlock *Instruction::getBlock() const { return _block; }

std::string Instruction::getName() const { return "%v" + std::to_string(id); }

std::string Instruction::str() const { return getName(); }

AllocaInst::AllocaInst(BasicBlock *block, Type *type)
  : Instruction(block, new PointerType(type)) {
}

std::string AllocaInst::str() const {
  return getName() + " = alloca " + type->baseType()->str();
}

BranchInst::BranchInst(BasicBlock *block, BasicBlock *dest)
  : Instruction(block, BasicType::VOID, {dest}) {
}

BranchInst::BranchInst(BasicBlock *block, Value *cond, BasicBlock *ifTrue,
                       BasicBlock *ifFalse)
  : Instruction(block, BasicType::VOID, {cond, ifTrue, ifFalse}) {
}

bool BranchInst::isConditional() const { return size() == 3; }

std::string BranchInst::str() const {
  if (isConditional()) {
    auto cond = getOperand<Value>(0);
    auto ifTrue = getOperand<BasicBlock>(1);
    auto ifFalse = getOperand<BasicBlock>(2);
    return "br i1 " + cond->getName() + ", label %" + ifTrue->str() +
           ", label %" + ifFalse->str();
  }
  auto dest = getOperand<BasicBlock>(0);
  return "br label %" + dest->str();
}

CallInst::CallInst(BasicBlock *block, Function *func,
                   std::initializer_list<Value *> params)
  : Instruction(block, func->getType(), {func}) {
  for (auto param : params)
    insert(new Use(this, param));
}

CallInst::CallInst(BasicBlock *block, Function *func,
                   std::vector<Value *> &params)
  : Instruction(block, func->getType(), {func}) {
  for (auto param : params)
    insert(new Use(this, param));
}

std::string CallInst::str() const {
  std::stringstream ss;
  ss << "(";
  for (int i = 1; i < operands.size(); i++) {
    if (i != 1)
      ss << ", ";
    auto param = getOperand<Value>(i);
    ss << param->getType()->str() << " " << param->getName();
  }
  ss << ")";

  auto func = getOperand<Function>(0);
  if (func->getType() == BasicType::VOID)
    return "call " + func->getType()->str() + " " + func->getName() +
           ss.str();
  return getName() + " = call " + func->getType()->str() + " " +
         func->getName() + ss.str();
}

Type *GetElementPtrInst::calcType(Value *value, size_t indexSize) {
  auto type = value->getType();
  if (dynamic_cast<GlobalVariable *>(value))
    type = new PointerType(type);

  for (int i = 0; i < indexSize; i++)
    type = type->baseType();
  return new PointerType(type);
}

GetElementPtrInst::GetElementPtrInst(BasicBlock *block, Value *ptr,
                                     std::initializer_list<Value *> indexes)
  : Instruction(block, calcType(ptr, indexes.size()), {ptr}) {
  for (const auto index : indexes)
    insert(new Use(this, index));
}

GetElementPtrInst::GetElementPtrInst(BasicBlock *block, Value *ptr,
                                     std::vector<Value *> &indexes)
  : Instruction(block, calcType(ptr, indexes.size()), {ptr}) {
  for (const auto index : indexes)
    insert(new Use(this, index));
}

std::string GetElementPtrInst::str() const {
  auto ptr = getOperand<Value>(0);
  auto tempPtr = new PointerType(ptr->getType());
  std::stringstream ss;
  ss << getName() << " = getelementptr "
      << (dynamic_cast<GlobalVariable *>(ptr)
            ? ptr->getType()->str()
            : ptr->getType()->baseType()->str())
      << ", "
      << (dynamic_cast<GlobalVariable *>(ptr)
            ? tempPtr->str()
            : ptr->getType()->str())
      << " " << ptr->getName();
  delete tempPtr;
  for (int i = 1; i < size(); i++) {
    auto operand = getOperand<Value>(i);
    ss << ", " << operand->getType()->str() << " " << operand->getName();
  }

  return ss.str();
}

LoadInst::LoadInst(BasicBlock *block, Value *ptr)
  : Instruction(block,
                dynamic_cast<GlobalVariable *>(ptr)
                  ? ptr->getType()
                  : ptr->getType()->baseType(),
                {ptr}) {
}

std::string LoadInst::str() const {
  auto ptr = getOperand<Value>(0);
  return getName() + " = load " + type->str() + ", " +
         (dynamic_cast<GlobalVariable *>(ptr)
            ? (new PointerType(ptr->getType()))->str()
            : ptr->getType()->str()) +
         " " + ptr->getName();
}

PHINode::PHINode(BasicBlock *block, Type *type) : Instruction(block, type) {
}

void PHINode::add(BasicBlock *block, Use *use) {
  User::insert(use);
  _useBlockMap[use] = block;
}

std::pair<BasicBlock *, Value *> PHINode::getBlockValue(int index) {
  auto use = get(index);
  if (_useBlockMap.find(use) != _useBlockMap.end())
    return {_useBlockMap[use], use->getValue()};
  else
    throw std::runtime_error("Failed to get the use from this index.");
}

void PHINode::setBlockValue(int index, BasicBlock *block) {
  auto use = get(index);
  if (use != nullptr)
    _useBlockMap[use] = block;
  else
    throw std::runtime_error("Failed to get the use from this index.");
}

std::string PHINode::str() const {
  std::stringstream ss;
  for (int i = 0; i < operands.size(); i++) {
    if (i != 0)
      ss << ", ";

    ss << "[" << operands[i]->getValue()->getName() << ", %"
        << _useBlockMap.at(operands[i])->str() << "]";
  }
  return getName() + " = phi " + type->str() + " " + ss.str();
}

RetInst::RetInst(BasicBlock *block) : Instruction(block, BasicType::VOID) {
}

RetInst::RetInst(BasicBlock *block, Value *retValue)
  : Instruction(block, BasicType::VOID, {retValue}) {
}

std::string RetInst::str() const {
  if (empty())
    return "ret void";
  auto retValue = getOperand<Value>(0);
  return "ret " + retValue->getType()->str() + " " + retValue->getName();
}

StoreInst::StoreInst(BasicBlock *block, Value *value, Value *pointer)
  : Instruction(block, BasicType::VOID, {value, pointer}) {
}

std::string StoreInst::str() const {
  auto value = getOperand<Value>(0);
  auto pointer = getOperand<Value>(1);
  if (auto globalVar = dynamic_cast<GlobalVariable *>(pointer)) {
    return "store " + value->getType()->str() + " " + value->getName() + ", " +
           globalVar->getType()->str() + " " + pointer->getName();
  }
  return "store " + value->getType()->str() + " " + value->getName() + ", " +
         pointer->getType()->str() + " " + pointer->getName();
}

BinaryOperator::BinaryOperator(BasicBlock *block, Op op, Value *lhs, Value *rhs)
  : Instruction(block, Type::checkEquality(lhs->getType(), rhs->getType()), {lhs, rhs}),
    op(op) {
}

std::string BinaryOperator::str() const {
  auto lhs = getOperand<Value>(0);
  auto rhs = getOperand<Value>(1);
  auto type = Type::checkEquality(lhs->getType(), rhs->getType());
  return getName() + " = " + _opToString(op) + " " + type->str() + " " +
         lhs->getName() + ", " + rhs->getName();
}

std::string BinaryOperator::_opToString(Op v) noexcept {
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
    return "incomplete{_opToString}";
  };
}

CmpInst::CmpInst(BasicBlock *block, Cond cond, Value *lhs, Value *rhs)
  : Instruction(block, BasicType::I1), cond(cond) {
  insert(new Use(this, lhs));
  insert(new Use(this, rhs));
}

CmpInst::Cond CmpInst::getCond() const { return cond; }

std::string CmpInst::str() const {
  auto cond = getCond();
  auto class_name = getClassName();
  auto op1 = getOperand<Value>(0);
  auto op2 = getOperand<Value>(1);
  auto type = Type::checkEquality(op1->getType(), op2->getType());
  return getName() + " = " + class_name + " " + _condToString(cond) + " " +
         type->str() + " " + op1->getName() + ", " + op2->getName();
}

std::string CmpInst::getClassName() const {
  return "incomplete{CmpInst}";
}


std::string CmpInst::_condToString(Cond v) {
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
      return "incomplete{_condToString}";
  };
}

FCmpInst::FCmpInst(BasicBlock *block, Cond cond, Value *lhs, Value *rhs)
  : CmpInst(block, cond, lhs, rhs) {
}

std::string FCmpInst::getClassName() const { return "fcmp"; }

ICmpInst::ICmpInst(BasicBlock *block, Cond cond, Value *lhs, Value *rhs)
  : CmpInst(block, cond, lhs, rhs) {
}

std::string ICmpInst::getClassName() const { return "icmp"; }

CastInst::CastInst(BasicBlock *block, Type *type, Value *operand)
  : Instruction(block, type, {operand}) {
  insert(new Use(this, operand));
}

std::string CastInst::str() const {
  auto operand = getOperand<Value>(0);
  auto class_name = getClassName();
  return getName() + " = " + class_name + " " + operand->getType()->str() +
         " " + operand->getName() + " to " + type->str();
}

std::string CastInst::getClassName() const {
  return "incomplete{CastInst}";
}

BitCastInst::BitCastInst(BasicBlock *block, Type *type, Value *operand)
  : CastInst(block, type, operand) {
}

std::string BitCastInst::getClassName() const {
    return "bitcast";
}

FPToSIInst::FPToSIInst(BasicBlock *block, Type *type, Value *operand)
  : CastInst(block, type, operand) {
}

std::string FPToSIInst::getClassName() const {
    return "fptosi";
}

SExtInst::SExtInst(BasicBlock *block, Type *type, Value *operand)
  : CastInst(block, type, operand) {
}

std::string SExtInst::getClassName() const {
    return "sext";
}

SIToFPInst::SIToFPInst(BasicBlock *block, Type *type, Value *operand)
  : CastInst(block, type, operand) {
}

std::string SIToFPInst::getClassName() const {
    return "sitofp";
}

ZExtInst::ZExtInst(BasicBlock *block, Type *type, Value *operand)
  : CastInst(block, type, operand) {
}

std::string ZExtInst::getClassName() const { return "zext"; }

} // namespace ir