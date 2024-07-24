#include "ir/global_variable.hpp"
#include <stdexcept>

namespace ir {

GlobalVariable::GlobalVariable(bool isConst, Type *type, std::string name,
                               Constant *value)
    : Value(type), _name(name), _isConst(isConst), value(value) {}

bool GlobalVariable::isConst() const { return _isConst; }

std::vector<int> GlobalVariable::getDimensions() {
  std::vector<int> dimensions;
  Type *curType = this->type;
  while (auto arrayType = dynamic_cast<ArrayType *>(curType)) {
    dimensions.push_back(arrayType->getArraySize());
    curType = arrayType->baseType();
  }
  return dimensions;
}

bool GlobalVariable::isSingle() {
  return dynamic_cast<BasicType *>(this->type) != nullptr;
}

bool GlobalVariable::isInBss() {
  return dynamic_cast<ConstantZero *>(this->value) != nullptr;
}

float GlobalVariable::getFloat() {
  if (auto castType = dynamic_cast<BasicType *>(this->type)) {
    if (auto castValue = dynamic_cast<ConstantNumber *>(this->value)) {
      return castValue->floatValue();
    }
  }
  throw std::runtime_error("Not a float");
}

float GlobalVariable::getFloat(int index) {
  std::vector<int> dimensions = getDimensions();
  Constant *curValue = value;
  for (int dimension : dimensions) {
    if (auto constantArray = dynamic_cast<ConstantArray *>(curValue)) {
      curValue = constantArray->getValues()[index / dimension];
      index = index % dimension;
    } else if (auto constantZero = dynamic_cast<ConstantZero *>(curValue)) {
      return 0;
    } else {
      throw std::runtime_error("Unexpected value");
    }
  }
  if (auto castValue = dynamic_cast<ConstantNumber *>(curValue)) {
    return castValue->floatValue();
  }
  throw std::runtime_error("Not a float");
}

int GlobalVariable::getInt() {
  if (auto castType = dynamic_cast<BasicType *>(this->type)) {
    if (auto castValue = dynamic_cast<ConstantNumber *>(this->value)) {
      return castValue->intValue();
    }
  }
  throw std::runtime_error("Not an int");
}

int GlobalVariable::getInt(int index) {
  std::vector<int> dimensions = getDimensions();
  dimensions.erase(dimensions.begin());
  dimensions.push_back(1);
  Constant *curValue = value;
  for (int dimension : dimensions) {
    if (auto constantArray = dynamic_cast<ConstantArray *>(curValue)) {
      curValue = constantArray->getValues()[index / dimension];
      index = index % dimension;
    } else if (auto constantZero = dynamic_cast<ConstantZero *>(curValue)) {
      return 0;
    } else {
      throw std::runtime_error("Unexpected value");
    }
  }
  if (auto castValue = dynamic_cast<ConstantNumber *>(curValue)) {
    return castValue->intValue();
  }
  throw std::runtime_error("Not an int");
}

Constant *GlobalVariable::getValue() { return value; }

std::string GlobalVariable::getName() const { return "@" + _name; }

std::string GlobalVariable::getRawName() const { return _name; }

std::string GlobalVariable::toString() const {
  return getName() + " = global " + value->toString();
}

} // namespace ir
