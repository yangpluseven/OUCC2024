#ifndef IR_VALUE_HPP
#define IR_VALUE_HPP

#include "type.hpp"
#include <memory>
#include <unordered_set>

namespace ir {

class Value {
private:
  std::unordered_set<Use *> uses;

protected:
  Type *const type;

public:
  Value(Type *type) : type(type) {}

  virtual ~Value() {
    // TODO: Implement destructor
  }

  Type *const getType() { return type; }

  int getSize() const { return type->getSize(); };

  void addUse(Use *use) { uses.insert(use); }

  void replaceAllUseAs(Value *value) {
    for (auto use : uses) {
      value->uses.insert(use);
      use->setValue(value);
    }
    uses.clear();
  }

  std::unordered_set<Use *> &getUses() { return uses; }

  virtual std::string getName() const = 0;
};

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

class Use {
private:
  User *user;
  Value *value;

public:
  Use(User *user, Value *value) : user(user), value(value) {}
  ~Use() {
    // TODO: Implement destructor
  }
  User *getUser() { return user; }
  Value *getValue() { return value; }
  void setUser(User *user) { this->user = user; }
  void setValue(Value *value) { this->value = value; }
};

} // namespace ir
#endif // IR_VALUE_HPP
