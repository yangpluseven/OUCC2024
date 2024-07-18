#ifndef IR_VALUE_HPP
#define IR_VALUE_HPP

#include "type.hpp"
#include "use.hpp"
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

  virtual std::string &getName() = 0;

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
};

} // namespace ir
#endif // IR_VALUE_HPP
