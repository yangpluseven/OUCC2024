#include "value.hpp"

namespace ir {

Use::Use(User *user, Value *value) : user(user), value(value) {
}

User *Use::getUser() const { return user; }

Value *Use::getValue() const { return value; }

void Use::setUser(User *user) { this->user = user; }

void Use::setValue(Value *value) { this->value = value; }

User::User(Type *type) : Value(type) {
}

size_t User::size() const { return operands.size(); }

void User::insert(Use *use) { insert(static_cast<int>(size()), use); }

void User::insert(int index, Use *use) {
  while (index >= operands.size())
    operands.push_back(nullptr);
  operands[index] = use;
  use->getValue()->addUse(use);
}

Use *User::erase(int index) {
  Use *use = get(index);
  if (use == nullptr) {
    return nullptr;
  }
  use->getValue()->getUses().erase(use);
  operands.erase(operands.begin() + index);
  return use;
}

Use *User::erase(const Value *value) {
  for (auto i = 0; i < operands.size(); i++) {
    if (operands[i]->getValue() == value) {
      return erase(i);
    }
  }
  return nullptr;
}

Use *User::get(int index) const {
  if (index < 0 || index >= operands.size()) {
    return nullptr;
  }
  return operands[index];
}

bool User::empty() const { return operands.empty(); }

void User::set(int index, Use *use) {
  while (index >= operands.size())
    operands.push_back(nullptr);
  operands[index] = use;
  use->getValue()->addUse(use);
}

Value::Value(Type *type) : type(type) {
}

Type *Value::getType() const { return type; }

size_t Value::getSize() const { return type->getSize(); }

void Value::addUse(Use *use) { _uses.insert(use); }

void Value::replaceAllUseAs(Value *value) {
  for (auto use : _uses) {
    value->_uses.insert(use);
    use->setValue(value);
  }
  _uses.clear();
}

std::unordered_set<Use *> &Value::getUses() { return _uses; }

} // namespace ir