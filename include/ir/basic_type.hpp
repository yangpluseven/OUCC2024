#ifndef IR_BASIC_TYPE_HPP
#define IR_BASIC_TYPE_HPP

#include "type.hpp"
#include <exception>
#include <string>

namespace ir {

class BasicType : public Type {
public:
  static BasicType* const I1;
  static BasicType* const I32;
  static BasicType* const FLOAT;
  static BasicType* const VOID;

  size_t getSize() override { return size; }

  std::string toString() override { return name; }

  Type* baseType() override { throw "UnsupportedOperationException"; }

private:
  int size;
  std::string name;

  BasicType(int size, std::string name) : size(size), name(name) {};
};

BasicType* const BasicType::I1 = new BasicType(1, "i1");
BasicType* const BasicType::I32 = new BasicType(32, "i32");
BasicType* const BasicType::FLOAT = new BasicType(32, "float");
BasicType* const BasicType::VOID = new BasicType(0, "void");

} // namespace ir
#endif