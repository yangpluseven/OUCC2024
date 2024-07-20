#ifndef CONSTANT_ARRAY_HPP
#define CONSTANT_ARRAY_HPP

#include "constant.hpp"
#include <vector>
#include <string>

namespace ir {

class ConstantArray : public Constant {
private:
  std::vector<Constant *> values;

public:
  ConstantArray(Type *type, std::vector<Constant *>&& values) : Constant(type), values(std::move(values)) {}

  std::vector<Constant *> &getValues() { return values; }

  std::string toString() override {
    // TODO values->toString()
    return type->toString();
  }

  std::string getName() const override {
    // TODO
  }
};

} // namespace ir

#endif