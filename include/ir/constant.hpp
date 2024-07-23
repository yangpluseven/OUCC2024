#ifndef CONSTANT_HPP
#define CONSTANT_HPP

#include "model/number.hpp"
#include "value.hpp"
#include <stdexcept>
#include <variant>

namespace ir {

class Constant : public User {
public:
  Constant(Type *type) : User(type) {}

  virtual ~Constant() {}

  virtual std::string toString() const = 0;

  virtual std::string getName() const = 0;
};

class ConstantNumber : public Constant {
private:
  std::variant<int, float> _value;

  static BasicType *determineType(const model::Number &num) {
    if (dynamic_cast<const model::IntNumber *>(&num))
      return BasicType::I32;
    else if (dynamic_cast<const model::FloatNumber *>(&num))
      return BasicType::FLOAT;
    else
      throw std::runtime_error("Unsupported value");
  }

  static std::variant<int, float> convertValue(const model::Number &num) {
    if (dynamic_cast<const model::IntNumber *>(&num))
      return num.intValue();
    else if (dynamic_cast<const model::FloatNumber *>(&num))
      return num.floatValue();
    else
      throw std::runtime_error("Unsupported value");
  }

public:
  explicit ConstantNumber(bool value)
      : Constant(BasicType::I1), _value(value ? 1 : 0) {}

  ConstantNumber(const model::Number &num)
      : Constant(determineType(num)), _value(convertValue(num)) {}

  model::Number *getValue() const {
    if (type == BasicType::I32)
      return new model::IntNumber(intValue());
    else if (type == BasicType::FLOAT)
      return new model::FloatNumber(floatValue());
    else
      throw std::runtime_error("Unexpected type");
  }

  float floatValue() const { return std::get<float>(_value); }

  int intValue() const { return std::get<int>(_value); }

  std::string getName() const override {
    if (type == BasicType::I1)
      return intValue() ? "true" : "false";
    else if (type == BasicType::I32)
      return std::to_string(intValue());
    else if (type == BasicType::FLOAT) {
      auto value = floatValue();
      return reinterpret_cast<const char *>(&value);
    } else
      throw std::runtime_error("Unexpected type");
  }

  ConstantNumber operator+(const ConstantNumber &rhs) {
    if (type == BasicType::I32)
      return ConstantNumber(model::IntNumber(intValue() + rhs.intValue()));
    else if (type == BasicType::FLOAT)
      return ConstantNumber(
          model::FloatNumber(floatValue() + rhs.floatValue()));
    else
      throw std::runtime_error("Unsupported type");
  }

  ConstantNumber operator-(const ConstantNumber &rhs) {
    if (type == BasicType::I32)
      return ConstantNumber(model::IntNumber(intValue() - rhs.intValue()));
    else if (type == BasicType::FLOAT)
      return ConstantNumber(
          model::FloatNumber(floatValue() - rhs.floatValue()));
    else
      throw std::runtime_error("Unsupported type");
  }

  ConstantNumber operator*(const ConstantNumber &rhs) {
    if (type == BasicType::I32)
      return ConstantNumber(model::IntNumber(intValue() * rhs.intValue()));
    else if (type == BasicType::FLOAT)
      return ConstantNumber(
          model::FloatNumber(floatValue() * rhs.floatValue()));
    else
      throw std::runtime_error("Unsupported type");
  }

  ConstantNumber operator/(const ConstantNumber &rhs) {
    if (type == BasicType::I32)
      return ConstantNumber(model::IntNumber(intValue() + rhs.intValue()));
    else if (type == BasicType::FLOAT)
      return ConstantNumber(
          model::FloatNumber(floatValue() / rhs.floatValue()));
    else
      throw std::runtime_error("Unsupported type");
  }

  ConstantNumber operator%(const ConstantNumber &rhs) {
    if (type == BasicType::I32)
      return ConstantNumber(model::IntNumber(intValue() % rhs.intValue()));
    else
      throw std::runtime_error("Unsupported type");
  }

  ConstantNumber operator^(const ConstantNumber &rhs) {
    if (type == BasicType::I32)
      return ConstantNumber(model::IntNumber(intValue() ^ rhs.intValue()));
    else if (type == BasicType::I1)
      return ConstantNumber((intValue() ^ rhs.intValue()) != 0);
    else
      throw std::runtime_error("Unsupported type");
  }

  ConstantNumber operator-() {
    if (type == BasicType::I32)
      return ConstantNumber(model::IntNumber(-intValue()));
    else if (type == BasicType::FLOAT)
      return ConstantNumber(model::FloatNumber(-floatValue()));
    else
      throw std::runtime_error("Unsupported type");
  }

  ConstantNumber operator!() {
    if (type == BasicType::I1 || type == BasicType::I32)
      return ConstantNumber(intValue() == 0);
    else if (type == BasicType::FLOAT)
      return ConstantNumber(floatValue() == 0.0f);
    else
      throw std::runtime_error("Unsupported type");
  }

  ConstantNumber operator==(const ConstantNumber &rhs) {
    if (type == BasicType::I32 || type == BasicType::I1)
      return ConstantNumber(intValue() == rhs.intValue());
    else if (type == BasicType::FLOAT)
      return ConstantNumber(floatValue() == rhs.floatValue());
    else
      throw std::runtime_error("Unsupported type");
  }

  ConstantNumber operator!=(const ConstantNumber &rhs) {
    if (type == BasicType::I32 || type == BasicType::I1)
      return ConstantNumber(intValue() != rhs.intValue());
    else if (type == BasicType::FLOAT)
      return ConstantNumber(floatValue() != rhs.floatValue());
    else
      throw std::runtime_error("Unsupported type");
  }

  ConstantNumber operator>(const ConstantNumber &rhs) {
    if (type == BasicType::I32 || type == BasicType::I1)
      return ConstantNumber(intValue() > rhs.intValue());
    else if (type == BasicType::FLOAT)
      return ConstantNumber(floatValue() > rhs.floatValue());
    else
      throw std::runtime_error("Unsupported type");
  }

  ConstantNumber operator>=(const ConstantNumber &rhs) {
    if (type == BasicType::I32 || type == BasicType::I1)
      return ConstantNumber(intValue() >= rhs.intValue());
    else if (type == BasicType::FLOAT)
      return ConstantNumber(floatValue() >= rhs.floatValue());
    else
      throw std::runtime_error("Unsupported type");
  }

  ConstantNumber operator<(const ConstantNumber &rhs) {
    if (type == BasicType::I32 || type == BasicType::I1)
      return ConstantNumber(intValue() < rhs.intValue());
    else if (type == BasicType::FLOAT)
      return ConstantNumber(floatValue() < rhs.floatValue());
    else
      throw std::runtime_error("Unsupported type");
  }

  ConstantNumber operator<=(const ConstantNumber &rhs) {
    if (type == BasicType::I32 || type == BasicType::I1)
      return ConstantNumber(intValue() <= rhs.intValue());
    else if (type == BasicType::FLOAT)
      return ConstantNumber(floatValue() <= rhs.floatValue());
    else
      throw std::runtime_error("Unsupported type");
  }

  ConstantNumber *add(ConstantNumber *rhs) {
    if (type == BasicType::I32)
      return new ConstantNumber(model::IntNumber(intValue() + rhs->intValue()));
    else if (type == BasicType::FLOAT)
      return new ConstantNumber(
          model::FloatNumber(floatValue() + rhs->floatValue()));
    else
      throw std::runtime_error("Unsupported type");
  }

  ConstantNumber *sub(ConstantNumber *rhs) {
    if (type == BasicType::I32)
      return new ConstantNumber(model::IntNumber(intValue() - rhs->intValue()));
    else if (type == BasicType::FLOAT)
      return new ConstantNumber(
          model::FloatNumber(floatValue() - rhs->floatValue()));
    else
      throw std::runtime_error("Unsupported type");
  }

  ConstantNumber *mul(ConstantNumber *rhs) {
    if (type == BasicType::I32)
      return new ConstantNumber(model::IntNumber(intValue() * rhs->intValue()));
    else if (type == BasicType::FLOAT)
      return new ConstantNumber(
          model::FloatNumber(floatValue() * rhs->floatValue()));
    else
      throw std::runtime_error("Unsupported type");
  }

  ConstantNumber *div(ConstantNumber *rhs) {
    if (type == BasicType::I32)
      return new ConstantNumber(model::IntNumber(intValue() / rhs->intValue()));
    else if (type == BasicType::FLOAT)
      return new ConstantNumber(
          model::FloatNumber(floatValue() / rhs->floatValue()));
    else
      throw std::runtime_error("Unsupported type");
  }

  ConstantNumber *rem(ConstantNumber *rhs) {
    if (type == BasicType::I32)
      return new ConstantNumber(model::IntNumber(intValue() % rhs->intValue()));
    else
      throw std::runtime_error("Unsupported type");
  }

  ConstantNumber *exor(ConstantNumber *rhs) {
    if (type == BasicType::I32)
      return new ConstantNumber(model::IntNumber(intValue() ^ rhs->intValue()));
    else if (type == BasicType::I1)
      return new ConstantNumber((intValue() ^ rhs->intValue()) != 0);
    else
      throw std::runtime_error("Unsupported type");
  }

  ConstantNumber *neg() {
    if (type == BasicType::I32)
      return new ConstantNumber(model::IntNumber(-intValue()));
    else if (type == BasicType::FLOAT)
      return new ConstantNumber(model::FloatNumber(-floatValue()));
    else
      throw std::runtime_error("Unsupported type");
  }

  ConstantNumber *lnot(ConstantNumber *rhs) {
    if (type == BasicType::I32 || type == BasicType::I1)
      return new ConstantNumber(intValue() == 0);
    else if (type == BasicType::FLOAT)
      return new ConstantNumber(floatValue() == 0.0f);
    else
      throw std::runtime_error("Unsupported type");
  }

  ConstantNumber *eq(ConstantNumber *rhs) {
    if (type == BasicType::I32 || type == BasicType::I1)
      return new ConstantNumber(intValue() == rhs->intValue());
    else if (type == BasicType::FLOAT)
      return new ConstantNumber(floatValue() == rhs->floatValue());
    else
      throw std::runtime_error("Unsupported type");
  }

  ConstantNumber *ne(ConstantNumber *rhs) {
    if (type == BasicType::I32 || type == BasicType::I1)
      return new ConstantNumber(intValue() != rhs->intValue());
    else if (type == BasicType::FLOAT)
      return new ConstantNumber(floatValue() != rhs->floatValue());
    else
      throw std::runtime_error("Unsupported type");
  }

  ConstantNumber *gt(ConstantNumber *rhs) {
    if (type == BasicType::I32 || type == BasicType::I1)
      return new ConstantNumber(intValue() > rhs->intValue());
    else if (type == BasicType::FLOAT)
      return new ConstantNumber(floatValue() > rhs->floatValue());
    else
      throw std::runtime_error("Unsupported type");
  }

  ConstantNumber *ge(ConstantNumber *rhs) {
    if (type == BasicType::I32 || type == BasicType::I1)
      return new ConstantNumber(intValue() >= rhs->intValue());
    else if (type == BasicType::FLOAT)
      return new ConstantNumber(floatValue() >= rhs->floatValue());
    else
      throw std::runtime_error("Unsupported type");
  }

  ConstantNumber *lt(ConstantNumber *rhs) {
    if (type == BasicType::I32 || type == BasicType::I1)
      return new ConstantNumber(intValue() < rhs->intValue());
    else if (type == BasicType::FLOAT)
      return new ConstantNumber(floatValue() < rhs->floatValue());
    else
      throw std::runtime_error("Unsupported type");
  }

  ConstantNumber *le(ConstantNumber *rhs) {
    if (type == BasicType::I32 || type == BasicType::I1)
      return new ConstantNumber(intValue() <= rhs->intValue());
    else if (type == BasicType::FLOAT)
      return new ConstantNumber(floatValue() <= rhs->floatValue());
    else
      throw std::runtime_error("Unsupported type");
  }

  std::string toString() const override {
    return type->toString() + " " + getName();
  }
};

class ConstantZero : public Constant {
public:
  ConstantZero(Type *type) : Constant(type) {}

  std::string getName() const override { return "zeroinitializer"; }

  std::string toString() const override {
    return type->toString() + " zeroinitializer";
  }
};

class ConstantArray : public Constant {
private:
  std::vector<Constant *> values;

public:
  ConstantArray(Type *type, std::vector<Constant *> values)
      : Constant(type), values(values) {}

  std::vector<Constant *> &getValues() { return values; }

  std::string toString() const override {
    std::string result = "[";
    for (auto value : values) {
      result += value->toString() + ", ";
    }
    result += "]";
    return type->toString() + " " + result;
  }

  std::string getName() const override { return toString(); }
};

} // namespace ir

#endif