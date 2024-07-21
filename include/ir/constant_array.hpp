#ifndef CONSTANT_ARRAY_HPP
#define CONSTANT_ARRAY_HPP

#include "constant.hpp"
#include <string>
#include <vector>

namespace ir {

class ConstantArray : public Constant {
private:
  std::vector<Constant *> values;

public:
  ConstantArray(Type *type, std::vector<Constant *> values)
      : Constant(type), values(values) {}

  std::vector<Constant *> &getValues() { return values; }

  std::string toString() const override {
    std::string result = "[";
    for (auto value : values) {
      result += value->toString() + ", ";
    }
    result += "]";
    return type->toString() + " " + result;
  }

  std::string getName() const override { return toString(); }
};

} // namespace ir

#endif