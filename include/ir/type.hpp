#ifndef IR_TYPE_HPP
#define IR_TYPE_HPP

#include <memory>
#include <string>
#include <vector>

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

class BasicType : public Type {
public:
  static BasicType *const I1;
  static BasicType *const I32;
  static BasicType *const FLOAT;
  static BasicType *const VOID;

  size_t getSize() const override { return _size; }

  std::string toString() const override { return _name; }

  Type *baseType() const override { throw "UnsupportedOperationException"; }

private:
  size_t _size;
  std::string _name;

  BasicType(size_t size, std::string name) : _size(size), _name(name){};
};

BasicType *const BasicType::I1 = new BasicType(1, "i1");
BasicType *const BasicType::I32 = new BasicType(32, "i32");
BasicType *const BasicType::FLOAT = new BasicType(32, "float");
BasicType *const BasicType::VOID = new BasicType(0, "void");

class PointerType : public Type {
private:
  Type *const _bType;

public:
  PointerType(Type *baseType) : _bType(baseType) {}

  Type *baseType() const override { return _bType; }

  size_t getSize() const override { return 64; }

  std::string toString() const override { return baseType()->toString() + "*"; }
};

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