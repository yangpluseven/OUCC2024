#include "value.hpp"

namespace ir {

Use::Use(User *user, Value *value) : user(user), value(value) {}

User *Use::getUser() const { return user; }

Value *Use::getValue() const { return value; }

void Use::setUser(User *user) { this->user = user; }

void Use::setValue(Value *value) { this->value = value; }

User::User(Type *type) : Value(type) {}

size_t User::size() const { return useOperands.size(); }

void User::insert(Use *use) { insert(static_cast<int>(size()), use); }

void User::insert(int index, Use *use) {
  while (index > useOperands.size())
    useOperands.push_back(nullptr);
  useOperands.insert(useOperands.begin() + index, use);
  use->getValue()->insertUse(use);
}

void User::replaceOperandsFrom(
    const std::unordered_map<Value *, Value *> &replaceMap,
    const User *other) {
  for (int i = 0; i < other->size(); i++) {
    const auto operand = other->getOperand<Value>(i);
    set(i, new Use(this, replaceMap.at(operand)));
  }
}

Use *User::erase(int index) {
  Use *use = get(index);
  if (use == nullptr) {
    return nullptr;
  }
  use->getValue()->getUses().erase(use);
  useOperands.erase(useOperands.begin() + index);
  return use;
}

Use *User::erase(const Value *value) {
  for (auto i = 0; i < useOperands.size(); i++) {
    if (useOperands[i]->getValue() == value) {
      return erase(i);
    }
  }
  return nullptr;
}

Use *User::get(int index) const {
  if (index < 0 || index >= useOperands.size()) {
    return nullptr;
  }
  return useOperands[index];
}

bool User::empty() const { return useOperands.empty(); }

void User::set(int index, Use *use) {
  while (index >= useOperands.size())
    useOperands.push_back(nullptr);
  useOperands[index] = use;
  use->getValue()->insertUse(use);
}

Value::Value(Type *type) : type(type) {}

Type *Value::getType() const { return type; }

size_t Value::getSize() const { return type->getSize(); }

void Value::insertUse(Use *use) { _uses.insert(use); }

void Value::replaceAllUseAs(Value *value) {
  for (auto use : _uses) {
    value->insertUse(use);
    use->setValue(value);
  }
  _uses.clear();
}

std::unordered_set<Use *> &Value::getUses() { return _uses; }

} // namespace ir