#ifndef IR_ARRAY_TYPE_HPP
#define IR_ARRAY_TYPE_HPP

#include "type.hpp"
#include <string>
#include <vector>

namespace ir {

class ArrayType : public Type {
private:
  Type *const _bType;
  size_t _arraySize;

public:
  ArrayType(Type *baseType, size_t arraySize)
      : _bType(baseType), _arraySize(arraySize) {}

  Type *baseType() const override { return _bType; }

  size_t getArraySize() { return _arraySize; }

  size_t getSize() const override { return baseType()->getSize() * _arraySize; }

  Type *getScalarType() {
    Type *type = this;
    while (auto arrayType = dynamic_cast<ArrayType *>(type)) {
      type = arrayType->baseType();
    }
    return type;
  }

  std::vector<ArrayType *> &getArrayTypes() {
    std::vector<ArrayType *> arrayTypes;
    Type *type = this;

    while (auto arrayType = dynamic_cast<ArrayType *>(type)) {
      arrayTypes.push_back(arrayType);
      type = arrayType->baseType();
    }
    return arrayTypes;
  }

  std::vector<size_t> &getArraySizes() {
    auto types = getArrayTypes();
    std::vector<size_t> arraySizes;
    for (auto i : types) {
      arraySizes.push_back(i->_arraySize);
    }
    return arraySizes;
  }

  std::string toString() const override {
    return "[" + std::to_string(_arraySize) + " x " + baseType()->toString() +
           "]";
  }
};

} // namespace ir

#endif