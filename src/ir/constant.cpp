#include "ir/constant.hpp"

namespace ir {

Constant::Constant(Type *type) : User(type) {}

Constant::~Constant() {}

BasicType *ConstantNumber::determineType(const model::Number &num) {
  auto value = num.getValue();
  if (std::holds_alternative<int>(value))
    return BasicType::I32;
  else if (std::holds_alternative<float>(value))
    return BasicType::FLOAT;
  else
    throw std::runtime_error("Unsupported value");
}

ConstantNumber::ConstantNumber(bool value)
    : Constant(BasicType::I1), _value(value ? 1 : 0) {}

ConstantNumber::ConstantNumber(const model::Number &num)
    : Constant(determineType(num)), _value(num) {}

ConstantNumber::ConstantNumber(const ConstantNumber &other)
    : Constant(other.type), _value(other._value) {}

ConstantNumber::ConstantNumber(ConstantNumber &&other) noexcept
    : Constant(other.type), _value(std::move(other._value)) {}

model::Number ConstantNumber::getValue() const { return _value; }

float ConstantNumber::floatValue() const { return _value.floatValue(); }

int ConstantNumber::intValue() const { return _value.intValue(); }

std::string ConstantNumber::getName() const {
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

ConstantNumber ConstantNumber::operator+(const ConstantNumber &rhs) {
  if (type == BasicType::I32)
    return ConstantNumber(model::Number(intValue() + rhs.intValue()));
  else if (type == BasicType::FLOAT)
    return ConstantNumber(model::Number(floatValue() + rhs.floatValue()));
  else
    throw std::runtime_error("Unsupported type");
}

ConstantNumber ConstantNumber::operator-(const ConstantNumber &rhs) {
  if (type == BasicType::I32)
    return ConstantNumber(model::Number(intValue() - rhs.intValue()));
  else if (type == BasicType::FLOAT)
    return ConstantNumber(model::Number(floatValue() - rhs.floatValue()));
  else
    throw std::runtime_error("Unsupported type");
}

ConstantNumber ConstantNumber::operator*(const ConstantNumber &rhs) {
  if (type == BasicType::I32)
    return ConstantNumber(model::Number(intValue() * rhs.intValue()));
  else if (type == BasicType::FLOAT)
    return ConstantNumber(model::Number(floatValue() * rhs.floatValue()));
  else
    throw std::runtime_error("Unsupported type");
}

ConstantNumber ConstantNumber::operator/(const ConstantNumber &rhs) {
  if (type == BasicType::I32)
    return ConstantNumber(model::Number(intValue() / rhs.intValue()));
  else if (type == BasicType::FLOAT)
    return ConstantNumber(model::Number(floatValue() / rhs.floatValue()));
  else
    throw std::runtime_error("Unsupported type");
}

ConstantNumber ConstantNumber::operator%(const ConstantNumber &rhs) {
  if (type == BasicType::I32)
    return ConstantNumber(model::Number(intValue() % rhs.intValue()));
  else
    throw std::runtime_error("Unsupported type");
}

ConstantNumber ConstantNumber::operator^(const ConstantNumber &rhs) {
  if (type == BasicType::I32)
    return ConstantNumber(model::Number(intValue() ^ rhs.intValue()));
  else if (type == BasicType::I1)
    return ConstantNumber((intValue() ^ rhs.intValue()) != 0);
  else
    throw std::runtime_error("Unsupported type");
}

ConstantNumber ConstantNumber::operator-() {
  if (type == BasicType::I32)
    return ConstantNumber(model::Number(-intValue()));
  else if (type == BasicType::FLOAT)
    return ConstantNumber(model::Number(-floatValue()));
  else
    throw std::runtime_error("Unsupported type");
}

ConstantNumber ConstantNumber::operator!() {
  if (type == BasicType::I1 || type == BasicType::I32)
    return ConstantNumber(intValue() == 0);
  else if (type == BasicType::FLOAT)
    return ConstantNumber(floatValue() == 0.0f);
  else
    throw std::runtime_error("Unsupported type");
}

ConstantNumber ConstantNumber::operator==(const ConstantNumber &rhs) {
  if (type == BasicType::I32 || type == BasicType::I1)
    return ConstantNumber(intValue() == rhs.intValue());
  else if (type == BasicType::FLOAT)
    return ConstantNumber(floatValue() == rhs.floatValue());
  else
    throw std::runtime_error("Unsupported type");
}

ConstantNumber ConstantNumber::operator!=(const ConstantNumber &rhs) {
  if (type == BasicType::I32 || type == BasicType::I1)
    return ConstantNumber(intValue() != rhs.intValue());
  else if (type == BasicType::FLOAT)
    return ConstantNumber(floatValue() != rhs.floatValue());
  else
    throw std::runtime_error("Unsupported type");
}

ConstantNumber ConstantNumber::operator>(const ConstantNumber &rhs) {
  if (type == BasicType::I32 || type == BasicType::I1)
    return ConstantNumber(intValue() > rhs.intValue());
  else if (type == BasicType::FLOAT)
    return ConstantNumber(floatValue() > rhs.floatValue());
  else
    throw std::runtime_error("Unsupported type");
}

ConstantNumber ConstantNumber::operator>=(const ConstantNumber &rhs) {
  if (type == BasicType::I32 || type == BasicType::I1)
    return ConstantNumber(intValue() >= rhs.intValue());
  else if (type == BasicType::FLOAT)
    return ConstantNumber(floatValue() >= rhs.floatValue());
  else
    throw std::runtime_error("Unsupported type");
}

ConstantNumber ConstantNumber::operator<(const ConstantNumber &rhs) {
  if (type == BasicType::I32 || type == BasicType::I1)
    return ConstantNumber(intValue() < rhs.intValue());
  else if (type == BasicType::FLOAT)
    return ConstantNumber(floatValue() < rhs.floatValue());
  else
    throw std::runtime_error("Unsupported type");
}

ConstantNumber ConstantNumber::operator<=(const ConstantNumber &rhs) {
  if (type == BasicType::I32 || type == BasicType::I1)
    return ConstantNumber(intValue() <= rhs.intValue());
  else if (type == BasicType::FLOAT)
    return ConstantNumber(floatValue() <= rhs.floatValue());
  else
    throw std::runtime_error("Unsupported type");
}

ConstantNumber *ConstantNumber::add(ConstantNumber *rhs) {
  return new ConstantNumber(*this + *rhs);
}

ConstantNumber *ConstantNumber::sub(ConstantNumber *rhs) {
  return new ConstantNumber(*this - *rhs);
}

ConstantNumber *ConstantNumber::mul(ConstantNumber *rhs) {
  return new ConstantNumber(*this * *rhs);
}

ConstantNumber *ConstantNumber::div(ConstantNumber *rhs) {
  return new ConstantNumber(*this / *rhs);
}

ConstantNumber *ConstantNumber::rem(ConstantNumber *rhs) {
  return new ConstantNumber(*this % *rhs);
}

ConstantNumber *ConstantNumber::exor(ConstantNumber *rhs) {
  return new ConstantNumber(*this ^ *rhs);
}

ConstantNumber *ConstantNumber::neg() { return new ConstantNumber(-*this); }

ConstantNumber *ConstantNumber::lnot(ConstantNumber *rhs) {
  return new ConstantNumber(!*rhs);
}

ConstantNumber *ConstantNumber::eq(ConstantNumber *rhs) {
  return new ConstantNumber(intValue() == rhs->intValue());
}

ConstantNumber *ConstantNumber::ne(ConstantNumber *rhs) {
  return new ConstantNumber(intValue() != rhs->intValue());
}

ConstantNumber *ConstantNumber::gt(ConstantNumber *rhs) {
  return new ConstantNumber(intValue() > rhs->intValue());
}

ConstantNumber *ConstantNumber::ge(ConstantNumber *rhs) {
  return new ConstantNumber(intValue() >= rhs->intValue());
}

ConstantNumber *ConstantNumber::lt(ConstantNumber *rhs) {
  return new ConstantNumber(intValue() < rhs->intValue());
}

ConstantNumber *ConstantNumber::le(ConstantNumber *rhs) {
  return new ConstantNumber(intValue() <= rhs->intValue());
}

std::string ConstantNumber::toString() const {
  return type->toString() + " " + getName();
}

ConstantZero::ConstantZero(Type *type) : Constant(type) {}

std::string ConstantZero::getName() const { return "zeroinitializer"; }

std::string ConstantZero::toString() const {
  return type->toString() + " zeroinitializer";
}

ConstantArray::ConstantArray(Type *type, std::vector<Constant *> values)
    : Constant(type), values(values) {}

std::vector<Constant *> &ConstantArray::getValues() { return values; }

std::string ConstantArray::toString() const {
  std::string result = "[";
  for (auto value : values) {
    result += value->toString() + ", ";
  }
  result += "]";
  return type->toString() + " " + result;
}

std::string ConstantArray::getName() const { return toString(); }

} // namespace ir
