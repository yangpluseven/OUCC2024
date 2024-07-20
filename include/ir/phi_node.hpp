#ifndef IR_PHI_NODE_HPP
#define IR_PHI_NODE_HPP

#include "instruction.hpp"
#include "value.hpp"
#include <string>
#include <unordered_map>

namespace ir {
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
    if (useBlockMap.contains(use))
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
} // namespace ir

#endif