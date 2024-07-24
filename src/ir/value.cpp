#include "ir/value.hpp"

namespace ir {

Use::Use(User *user, Value *value) : user(user), value(value) {}

Use::~Use() {
  // TODO: Implement destructor
}

User *Use::getUser() { return user; }

Value *Use::getValue() { return value; }

void Use::setUser(User *user) { this->user = user; }

void Use::setValue(Value *value) { this->value = value; }

User::User(Type *type) : Value(type) {}

User::~User() {
  // TODO: Implement destructor
}

size_t User::size() const { return operands.size(); }

void User::add(Use *use) { add(size(), use); }

void User::add(int index, Use *use) {
  while (index >= operands.size())
    operands.push_back(nullptr);
  operands[index] = use;
  use->getValue()->addUse(use);
}

Use *User::remove(int index) {
  Use *use = get(index);
  use->getValue()->getUses().erase(use);
  operands.erase(operands.begin() + index);
  return use;
}

Use *User::remove(Value *value) {
  for (auto i = 0; i < operands.size(); i++) {
    if (operands[i]->getValue() == value) {
      return remove(i);
    }
  }
  return nullptr;
}

Use *User::get(int index) {
  if (index < 0 || index >= operands.size()) {
    return nullptr;
  }
  return operands[index];
}

template <typename T,
          typename std::enable_if<std::is_base_of<Value, T>::value>::type *>
T *User::getOperand(int index) const {
  return static_cast<T *>(operands[index]->getValue());
}

template <typename T,
          typename std::enable_if<std::is_base_of<Value, T>::value>::type *>
T *User::getLastOperand() const {
  return getOperand<T>(size() - 1);
}

bool User::isEmpty() const { return operands.empty(); }

void User::set(int index, Use *use) {
  while (index >= operands.size())
    operands.push_back(nullptr);
  operands[index] = use;
  use->getValue()->addUse(use);
}

Value::Value(Type *type) : type(type) {}

Value::~Value() {
  // TODO: Implement destructor
}

Type *const Value::getType() { return type; }

int Value::getSize() const { return type->getSize(); }

void Value::addUse(Use *use) { uses.insert(use); }

void Value::replaceAllUseAs(Value *value) {
  for (auto use : uses) {
    value->uses.insert(use);
    use->setValue(value);
  }
  uses.clear();
}

std::unordered_set<Use *> &Value::getUses() { return uses; }

} // namespace ir
