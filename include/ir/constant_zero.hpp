#ifndef IR_CONSTANT_ZERO_HPP
#define IR_CONSTANT_ZERO_HPP

#include "constant.hpp"
namespace ir {
class ConstantZero : public Constant {
public:
  ConstantZero(Type *type) : Constant(type) {}

  std::string getName() const override { return "zeroinitializer"; }

  std::string toString() const override {
    return type->toString() + " zeroinitializer";
  }
};
} // namespace ir
#endif