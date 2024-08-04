#include "global_variable.hpp"
#include <stdexcept>
#include <utility>

namespace ir {

GlobalVariable::GlobalVariable(bool isConst, Type *type, std::string name,
                               Constant *value)
    : Value(type), _name(std::move(name)), _isConst(isConst), value(value) {}

bool GlobalVariable::isConst() const { return _isConst; }

std::vector<int> GlobalVariable::getDimensions() const {
  std::vector<int> dimensions;
  Type *curType = this->type;
  while (auto arrayType = dynamic_cast<ArrayType *>(curType)) {
    dimensions.push_back(static_cast<int>(arrayType->getArraySize()));
    curType = arrayType->baseType();
  }
  return std::move(dimensions);
}

bool GlobalVariable::isSingle() const {
  return dynamic_cast<BasicType *>(this->type) != nullptr;
}

bool GlobalVariable::isInBss() const {
  return dynamic_cast<ConstantZero *>(this->value) != nullptr;
}

float GlobalVariable::getFloat() const {
  if (dynamic_cast<BasicType *>(this->type)) {
    if (auto castValue = dynamic_cast<ConstantNumber *>(this->value)) {
      return castValue->floatValue();
    }
  }
  throw std::runtime_error("Not a float");
}

float GlobalVariable::getFloat(int index) const {
  std::vector<int> dimensions = getDimensions();
  Constant *curValue = value;
  for (int dimension : dimensions) {
    if (auto constantArray = dynamic_cast<ConstantArray *>(curValue)) {
      curValue = constantArray->getValues()[index / dimension];
      index = index % dimension;
    } else if (dynamic_cast<ConstantZero *>(curValue)) {
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

int GlobalVariable::getInt() const {
  if (dynamic_cast<BasicType *>(this->type)) {
    if (auto castValue = dynamic_cast<ConstantNumber *>(this->value)) {
      return castValue->intValue();
    }
  }
  throw std::runtime_error("Not an int");
}

int GlobalVariable::getInt(int index) const {
  std::vector<int> dimensions = getDimensions();
  dimensions.erase(dimensions.begin());
  dimensions.push_back(1);
  Constant *curValue = value;
  for (int i = 0; i < dimensions.size(); i++) {
    const int dimension = dimensions[i];
    int num = 1;
    for (int j = i; j < dimensions.size(); j++) {
      num *= dimensions[j];
    }
    if (const auto constantArray = dynamic_cast<ConstantArray *>(curValue)) {
      curValue = constantArray->getValues()[index / num];
      index = index % dimension;
    } else if (dynamic_cast<ConstantZero *>(curValue)) {
      return 0;
    } else {
      throw std::runtime_error("Unexpected value");
    }
  }
  if (const auto castValue = dynamic_cast<ConstantNumber *>(curValue)) {
    return castValue->intValue();
  }
  throw std::runtime_error("Not an int");
}

Constant *GlobalVariable::getValue() const { return value; }

std::string GlobalVariable::getName() const { return "@" + _name; }

std::string GlobalVariable::getRawName() const { return _name; }

std::string GlobalVariable::str() const {
  return getName() + " = global " + value->str();
}

} // namespace ir
