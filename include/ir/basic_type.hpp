#ifndef IR_BASIC_TYPE_HPP
#define IR_BASIC_TYPE_HPP

#include "type.hpp"
#include <exception>
namespace ir {
class I1 : Type {
public:
  size_t getSize() override { return 1; }
  Type *baseType() { throw "UnsupportedOperation"; }
  std::string toString() override { return "i1"; }
};
class I32 : Type {
public:
  size_t getSize() override { return 32; }
  Type *baseType() { throw "UnsupportedOperation"; }
  std::string toString() override { return "i32"; }
};
class FLOAT : Type {
public:
  size_t getSize() override { return 32; }
  Type *baseType() { throw "UnsupportedOperation"; }
  std::string toString() override { return "float"; }
};
class VOID : Type {
public:
  size_t getSize() override { return 0; }
  Type *baseType() { throw "UnsupportedOperation"; }
  std::string toString() override { return "void"; }
};
} // namespace ir
#endif