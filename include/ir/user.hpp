#ifndef IR_USER_HPP
#define IR_USER_HPP
#include "value.hpp"
#include <vector>

namespace ir {

class User : public Value {
protected:
  std::vector<Use *> operands;

public:
  User(Type *type) : Value(type) {}
  virtual ~User() {
    // TODO: Implement destructor
  }
  size_t size() const { return operands.size(); }

  void add(Use *use) { add(size(), use); }

  void add(int index, Use *use) {
    while (index >= operands.size())
      operands.push_back(nullptr);
    operands[index] = use;
    use->getValue()->addUse(use);
  }

  Use *remove(int index) {
    Use *use = get(index);
    use->getValue()->getUses().erase(use);
    operands.erase(operands.begin() + index);
    return use;
  }

  Use *remove(Value *value) {
    for (auto i = 0; i < operands.size(); i++) {
      if (operands[i]->getValue() == value) {
        return remove(i);
      }
    }
    return nullptr;
  }

  Use *get(int index) {
    if (index < 0 || index >= operands.size()) {
      return nullptr;

      return operands[index];
    }
  }

  template <typename T, typename std::enable_if<
                            std::is_base_of<Value, T>::value>::type * = nullptr>
  T *getLastOperand() const {
    return getOperand(size() - 1);
  }
  template <typename T, typename std::enable_if<
                            std::is_base_of<Value, T>::value>::type * = nullptr>
  T *getOperand(int index) const {
    return static_cast<T *>(get(index)->getValue());
  }

  bool isEmpty() const { return operands.empty(); }

  void set(int index, Use *use) {
    while (index >= operands.size())
      operands.push_back(nullptr);
    operands[index] = use;
    use->getValue()->addUse(use);
  }
};
} // namespace ir
#endif // IR_USER_HPP