#ifndef IR_CONSTANT_NUMBER_HPP
#define IR_CONSTANT_NUMBER_HPP

#include "constant.hpp"

namespace ir {
template <typename T> class ConstantNumber : public Constant {
private:
  T _value;

  Type *typecast() const {
    if (std::is_same<T, int>::value) {
      return BasicType::I32;
    } else if (std::is_same<T, float>::value) {
      return BasicType::FLOAT;
    } else if (std::is_same<T, bool>::value) {
      return BasicType::I1;
    } else {
      throw "UnsupportedOperationException";
    }
  }

public:
  ConstantNumber(T value) : Constant(typecast()), _value(value) {}

  ConstantNumber(T value, Type *type) : Constant(type), _value(value) {}

  T getValue() const { return _value; }

  ConstantNumber &operator=(const) {
    _value = value;
    return *this;
  }

  ConstantNumber &operator+(const ConstantNumber &rhs) {
    return ConstantNumber(_value + rhs.getValue());
  }

  ConstantNumber &operator-(const ConstantNumber &rhs) {
    return ConstantNumber(_value - rhs.getValue());
  }

  ConstantNumber &operator*(const ConstantNumber &rhs) {
    return ConstantNumber(_value * rhs.getValue());
  }

  ConstantNumber &operator/(const ConstantNumber &rhs) {
    return ConstantNumber(_value / rhs.getValue());
  }

  ConstantNumber &operator^(const ConstantNumber &rhs) {
    return ConstantNumber(_value ^ rhs.getValue());
  }

  ConstantNumber &operator-() { return ConstantNumber(-_value); }

  ConstantNumber &operator==(const ConstantNumber &rhs) { if }

  ConstantNumber &operator!=(const ConstantNumber &rhs) {
    return ConstantNumber(_value != rhs.getValue());
  }

  ConstantNumber &operator<(const ConstantNumber &rhs) {
    return ConstantNumber(_value < rhs.getValue());
  }

  ConstantNumber &operator>(const ConstantNumber &rhs) {
    return ConstantNumber(_value > rhs.getValue());
  }

  ConstantNumber &operator<=(const ConstantNumber &rhs) {
    return ConstantNumber(_value <= rhs.getValue());
  }

  ConstantNumber &operator>=(const ConstantNumber &rhs) {
    return ConstantNumber(_value >= rhs.getValue());
  }

  std::string getName() override {
    if (std::is_same<T, int>::value)
      return std::to_string(_value);
    else if (std::is_same<T, float>::value)
      return reinterpret_cast<const char *>(&_value);
    else if (std::is_same<T, bool>::value)
      return _value ? "true" : "false";
    else
      throw "UnsupportedOperationException";
  }

  std::string toString() override { return type->toString() + " " + getName(); }
};
} // namespace ir

#endif