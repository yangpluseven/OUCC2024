#ifndef IR_TYPE_HPP
#define IR_TYPE_HPP
#include <memory>

namespace ir {

class Type {
public:
  Type() = default;

  virtual ~Type() {}

  virtual Type *baseType() = 0;

  virtual size_t getSize() = 0;

  virtual std::string toString() = 0;

  static Type *CheckEquality(Type *lhs, Type *rhs) {
    if (lhs->toString() != rhs->toString())
      throw "Unmatched types!";
    else
      return lhs;
  }
};

} // namespace ir
#endif