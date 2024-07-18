#ifndef IR_TYPE_HPP
#define IR_TYPE_HPP
#include <memory>

namespace ir {
class Type {
protected:
  Type *baseType = nullptr;

public:
  Type(Type *baseType) : baseType(baseType){};
  virtual ~Type() {}
  virtual size_t getSize() = 0;
  virtual Type *baseType() { return baseType; };
  virtual std::string toString() = 0;
};
} // namespace ir
#endif