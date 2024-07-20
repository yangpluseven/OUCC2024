#ifndef IR_POINTER_TYPE_HPP
#define IR_POINTER_TYPE_HPP

#include "type.hpp"
#include <exception>
#include <string>

namespace ir {

class PointerType : Type {
private:
  Type *const _bType;

public:
  PointerType(Type *baseType) : _bType(baseType) {}

  Type *baseType() override { return _bType; }

  size_t getSize() override { return 64; }

  std::string toString() override { return baseType()->toString() + "*"; }
};

} // namespace ir
#endif