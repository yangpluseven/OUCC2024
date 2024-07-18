#ifndef IR_ARRAY_TYPE_HPP
#define IR_ARRAY_TYPE_HPP

#include "type.hpp"
#include <vector>

namespace ir {

class ArrayType : Type {
public:
  size_t arraySize;
  ArrayType(Type *baseType, size_t arraySize)
      : Type(baseType), arraySize(arraySize) {}

  size_t getSize() override { return baseType()->getSize() * arraySize; }

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
      arraySizes.push_back(i->arraySize);
    }
    return arraySizes;
  }

  std::string toString() override {
    return "[" + std::to_string(arraySize) + " x " + baseType()->toString() + "]";
  }
};
} // namespace ir

#endif