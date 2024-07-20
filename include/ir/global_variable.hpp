#ifndef GLOABAL_VARIABLE_HPP
#define GLOBAL_VARIABLE_HPP
#include "array_type.hpp"
#include "basic_type.hpp"
#include "constant.hpp"
#include "constant_array.hpp"
#include "constant_number.hpp"
#include "constant_zero.hpp"
#include "user.hpp"
#include <string>

namespace ir {

class GlobalVariable : public Value {
private:
  std::string _name;
  bool _isConst;
  Constant *value;

public:
  GlobalVariable(bool isConst, Type *type, std::string name, Constant *value)
      : Value(type), _name(name), _isConst(isConst), value(value) {}

  bool isConst() const { return _isConst; }

  std::vector<int> getDimensions() {
    std::vector<int> dimensions;
    Type *curType = this->type;
    while (auto arrayType = dynamic_cast<ArrayType *>(curType)) {
      dimensions.push_back(arrayType->getArraySize());
      curType = arrayType->baseType();
    }
    return dimensions;
  }

  bool isSingle() {
    if (auto castType = dynamic_cast<BasicType *>(this->type)) {
      return true;
    } else {
      return false;
    }
  }

  bool isInBss() {
    if (auto castValue = dynamic_cast<ConstantZero *>(this->value)) {
      return true;
    } else {
      return false;
    }
  }

  float getFloat() {
    if (auto castType = dynamic_cast<BasicType *>(this->type)) {
      if (auto castValue = dynamic_cast<ConstantNumber<float> *>(this->value)) {
        return castValue->floatValue();
      }
    }
    throw "Not a float";
  }

  float getFloat(int index) {
    std::vector<int> dimensions = getDimensions();
    Constant *curValue = value;
    for (int dimension : dimensions) {
      if (auto constantArray = dynamic_cast<ConstantArray *>(curValue)) {
        curValue = constantArray->getValues()[index / dimension];
        index = index % dimension;
      } else if (auto constantZero = dynamic_cast<ConstantZero *>(curValue)) {
        return 0;
      } else {
        throw "Unexpected value";
      }
    }
    if (auto castValue = dynamic_cast<ConstantNumber<float> *>(curValue)) {
      return castValue->floatValue();
    }
    throw "Not a float";
  }

  int getInt() {
    if (auto castType = dynamic_cast<BasicType *>(this->type)) {
      if (auto castValue = dynamic_cast<ConstantNumber<int> *>(this->value)) {
        return castValue->intValue();
      }
    }
    throw "Not an int";
  }

  int getInt(int index) {
    std::vector<int> dimensions = getDimensions();
    dimensions.erase(dimensions.begin());
    dimensions.push_back(1);
    Constant *curValue = value;
    for (int i = 0; i < dimensions.size(); i++) {
      int dimension = dimensions[i];
      if (auto constantArray = dynamic_cast<ConstantArray *>(curValue)) {
        curValue = constantArray->getValues()[index / dimension];
        index = index % dimension;
      } else if (auto constantZero = dynamic_cast<ConstantZero *>(curValue)) {
        return 0;
      } else {
        throw "Unexpected value";
      }
    }
    if (auto castValue = dynamic_cast<ConstantNumber<int> *>(curValue)) {
      return castValue->intValue();
    }
    throw "Not an int";
  }

  Constant *getValue() { return value; }

  std::string getName() const override { return "@" + _name; }

  std::string getRawName() const { return _name; }

  std::string toString() const {
    return getName() + " = global " + value->toString();
  }
};

} // namespace ir

#endif // GLOABAL_VARIABLE_HPP