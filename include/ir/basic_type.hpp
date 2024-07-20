#ifndef IR_BASIC_TYPE_HPP
#define IR_BASIC_TYPE_HPP

#include "type.hpp"
#include <exception>
#include <string>

namespace ir {

class BasicType : public Type {
public:
  static BasicType *const I1;
  static BasicType *const I32;
  static BasicType *const FLOAT;
  static BasicType *const VOID;

  size_t getSize() override { return _size; }

  std::string toString() override { return _name; }

  Type *baseType() override { throw "UnsupportedOperationException"; }

private:
  size_t _size;
  std::string _name;

  BasicType(size_t size, std::string name) : _size(size), _name(name){};
};

BasicType *const BasicType::I1 = new BasicType(1, "i1");
BasicType *const BasicType::I32 = new BasicType(32, "i32");
BasicType *const BasicType::FLOAT = new BasicType(32, "float");
BasicType *const BasicType::VOID = new BasicType(0, "void");

} // namespace ir
#endif