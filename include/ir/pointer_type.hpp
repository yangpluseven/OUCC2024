#ifndef IR_POINTER_TYPE_HPP
#define IR_POINTER_TYPE_HPP

#include "type.hpp"
#include <exception>
namespace ir {
class PointerType : Type {
public:
  PointerType(Type *baseType) : Type(baseType) {}
  size_t getSize() override { return 64; }
  std::string toString() override { return baseType()->toString() + "*"; }
};
} // namespace ir
#endif