#include "ir/type.hpp"
#include <sstream>
#include <stdexcept>
#include <utility>

namespace ir {

Type::~Type() = default;

Type *Type::checkEquality(Type *lhs, const Type *rhs) {
  if (lhs == nullptr && rhs == nullptr)
    return nullptr;
  if (lhs == nullptr || rhs == nullptr)
    throw std::runtime_error("Unmatched types!");
  if (lhs == rhs) {
    return lhs;
  }
  if (lhs->toString() != rhs->toString())
    throw std::runtime_error("Unmatched types!");
  return lhs;
}

bool Type::operator==(const Type &rhs) const {
  return toString() == rhs.toString();
}

BasicType::BasicType(size_t size, std::string name)
    : _size(size), _name(std::move(name)) {}

size_t BasicType::getSize() const { return _size; }

std::string BasicType::toString() const { return _name; }

Type *BasicType::baseType() const {
  throw std::runtime_error("UnsupportedOperationException");
}

PointerType::PointerType(Type *baseType) : _bType(baseType) {}

Type *PointerType::baseType() const { return _bType; }

size_t PointerType::getSize() const { return 64; }

std::string PointerType::toString() const {
  return baseType()->toString() + "*";
}

ArrayType::ArrayType(Type *baseType, size_t arraySize)
    : _bType(baseType), _arraySize(arraySize) {}

Type *ArrayType::baseType() const { return _bType; }

size_t ArrayType::getArraySize() const { return _arraySize; }

size_t ArrayType::getSize() const { return baseType()->getSize() * _arraySize; }

Type *ArrayType::getScalarType() {
  Type *type = this;
  while (auto arrayType = dynamic_cast<ArrayType *>(type)) {
    type = arrayType->baseType();
  }
  return type;
}

std::vector<ArrayType *> ArrayType::getArrayTypes() {
  std::vector<ArrayType *> arrayTypes;
  Type *type = this;
  while (auto arrayType = dynamic_cast<ArrayType *>(type)) {
    arrayTypes.push_back(arrayType);
    type = arrayType->baseType();
  }
  return arrayTypes;
}

std::vector<size_t> ArrayType::getArraySizes() {
  std::vector<size_t> arraySizes;
  auto types = getArrayTypes();
  arraySizes.reserve(types.size());
  for (auto i : types) {
    arraySizes.push_back(i->_arraySize);
  }
  return arraySizes;
}

std::string ArrayType::toString() const {
  std::ostringstream oss;
  oss << "[" << _arraySize << " x " << baseType()->toString() << "]";
  return oss.str();
}

} // namespace ir