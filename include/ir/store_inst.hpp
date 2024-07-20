#ifndef STORE_INST_HPP
#define STORE_INST_HPP
#include "instruction.hpp"
#include "pointer_type.hpp"
#include "value.hpp"
#include "global_variable.hpp"

namespace ir {

class StoreInst : public Instruction {
public:
  StoreInst(BasicBlock *block, Value *value, Value *pointer)
    : Instruction(block, BasicType::VOID, {value, pointer}) {}

  std::string toString() {
    auto value = getOperand<Value>(0);
    auto pointer = getOperand<Value>(1);
    if (auto globalVar = dynamic_cast<GlobalVariable *>(pointer)) {
      return "store " + value->getType()->toString() + value->getName() + ", " + globalVar->getType()->toString() + pointer->getName();
    }
    return "store " + value->getType()->toString() + value->getName() + ", " + pointer->getType()->toString() + pointer->getName();
  }
};

}

#endif