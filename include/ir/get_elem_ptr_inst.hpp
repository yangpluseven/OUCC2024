#ifndef IR_GET_ELEM_PTR_INST_HPP
#define IR_GET_ELEM_PTR_INST_HPP

#include "global_variable.hpp"
#include "instruction.hpp"
#include "pointer_type.hpp"
#include <string>

namespace ir {

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
} // namespace ir

#endif