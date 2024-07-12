#pragma once

#include <memory>
#include <unordered_set>
#include "type.hpp"
#include "use.hpp"

namespace ir {

class Value {
private:
  std::unordered_set<std::shared_ptr<Use>> uses;

protected:
  const std::shared_ptr<Type> type;

public:
  Value(std::shared_ptr<Type> type) : type(type) {}
  std::shared_ptr<Type> getType() { return type; }
  int getSize() { return type->getSize(); }
  void addUse(std::shared_ptr<Use> use) { uses.insert(use); }
  void replaceAllUseAs(std::shared_ptr<Value>& value) {
    for (auto use : uses) {
      value->uses.insert(use);
      use->setValue(value);
    }
    uses.clear();
  }
  virtual std::string getName() = 0;
};

} // namespace ir
