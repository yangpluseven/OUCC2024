#ifndef IR_POINTER_TYPE_HPP
#define IR_POINTER_TYPE_HPP

#include "type.hpp"
#include <exception>
#include <string>

namespace ir {

class PointerType : public Type {
private:
  Type *const _bType;

public:
  PointerType(Type *baseType) : _bType(baseType) {}

  Type *baseType() const override { return _bType; }

  size_t getSize() const override { return 64; }

  std::string toString() const override { return baseType()->toString() + "*"; }
};

} // namespace ir
#endif