#ifndef IR_TYPE_HPP
#define IR_TYPE_HPP
#include <memory>
#include <string>

namespace ir {

class Type {
public:
  Type() = default;

  virtual ~Type() {}

  virtual Type *baseType() const = 0;

  virtual size_t getSize() const = 0;

  virtual std::string toString() const = 0;

  static Type *CheckEquality(Type *lhs, Type *rhs) {
    if (lhs->toString() != rhs->toString())
      throw "Unmatched types!";
    else
      return lhs;
  }

  bool operator==(const Type &rhs) const {
    return toString() == rhs.toString();
  }
};

} // namespace ir
#endif