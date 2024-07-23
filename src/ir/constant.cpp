#include "ir/constant.hpp"

namespace ir {

Constant::Constant(Type *type) : User(type) {}

Constant::~Constant() {}

BasicType *ConstantNumber::determineType(const model::Number &num) {
  if (dynamic_cast<const model::IntNumber *>(&num))
    return BasicType::I32;
  else if (dynamic_cast<const model::FloatNumber *>(&num))
    return BasicType::FLOAT;
  else
    throw std::runtime_error("Unsupported value");
}

std::variant<int, float> ConstantNumber::convertValue(const model::Number &num) {
  if (dynamic_cast<const model::IntNumber *>(&num))
    return num.intValue();
  else if (dynamic_cast<const model::FloatNumber *>(&num))
    return num.floatValue();
  else
    throw std::runtime_error("Unsupported value");
}

ConstantNumber::ConstantNumber(bool value)
    : Constant(BasicType::I1), _value(value ? 1 : 0) {}

ConstantNumber::ConstantNumber(const model::Number &num)
    : Constant(determineType(num)), _value(convertValue(num)) {}

model::Number *ConstantNumber::getValue() const {
  if (type == BasicType::I32)
    return new model::IntNumber(intValue());
  else if (type == BasicType::FLOAT)
    return new model::FloatNumber(floatValue());
  else
    throw std::runtime_error("Unexpected type");
}

float ConstantNumber::floatValue() const { return std::get<float>(_value); }

int ConstantNumber::intValue() const { return std::get<int>(_value); }

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
    return ConstantNumber(model::IntNumber(intValue() + rhs.intValue()));
  else if (type == BasicType::FLOAT)
    return ConstantNumber(
        model::FloatNumber(floatValue() + rhs.floatValue()));
  else
    throw std::runtime_error("Unsupported type");
}

ConstantNumber ConstantNumber::operator-(const ConstantNumber &rhs) {
  if (type == BasicType::I32)
    return ConstantNumber(model::IntNumber(intValue() - rhs.intValue()));
  else if (type == BasicType::FLOAT)
    return ConstantNumber(
        model::FloatNumber(floatValue() - rhs.floatValue()));
  else
    throw std::runtime_error("Unsupported type");
}

ConstantNumber ConstantNumber::operator*(const ConstantNumber &rhs) {
  if (type == BasicType::I32)
    return ConstantNumber(model::IntNumber(intValue() * rhs.intValue()));
  else if (type == BasicType::FLOAT)
    return ConstantNumber(
        model::FloatNumber(floatValue() * rhs.floatValue()));
  else
    throw std::runtime_error("Unsupported type");
}

ConstantNumber ConstantNumber::operator/(const ConstantNumber &rhs) {
  if (type == BasicType::I32)
    return ConstantNumber(model::IntNumber(intValue() / rhs.intValue()));
  else if (type == BasicType::FLOAT)
    return ConstantNumber(
        model::FloatNumber(floatValue() / rhs.floatValue()));
  else
    throw std::runtime_error("Unsupported type");
}

ConstantNumber ConstantNumber::operator%(const ConstantNumber &rhs) {
  if (type == BasicType::I32)
    return ConstantNumber(model::IntNumber(intValue() % rhs.intValue()));
  else
    throw std::runtime_error("Unsupported type");
}

ConstantNumber ConstantNumber::operator^(const ConstantNumber &rhs) {
  if (type == BasicType::I32)
    return ConstantNumber(model::IntNumber(intValue() ^ rhs.intValue()));
  else if (type == BasicType::I1)
    return ConstantNumber((intValue() ^ rhs.intValue()) != 0);
  else
    throw std::runtime_error("Unsupported type");
}

ConstantNumber ConstantNumber::operator-() {
  if (type == BasicType::I32)
    return ConstantNumber(model::IntNumber(-intValue()));
  else if (type == BasicType::FLOAT)
    return ConstantNumber(model::FloatNumber(-floatValue()));
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
  if (type == BasicType::I32)
    return new ConstantNumber(model::IntNumber(intValue() + rhs->intValue()));
  else if (type == BasicType::FLOAT)
    return new ConstantNumber(
        model::FloatNumber(floatValue() + rhs->floatValue()));
  else
    throw std::runtime_error("Unsupported type");
}

ConstantNumber *ConstantNumber::sub(ConstantNumber *rhs) {
  if (type == BasicType::I32)
    return new ConstantNumber(model::IntNumber(intValue() - rhs->intValue()));
  else if (type == BasicType::FLOAT)
    return new ConstantNumber(
        model::FloatNumber(floatValue() - rhs->floatValue()));
  else
    throw std::runtime_error("Unsupported type");
}

ConstantNumber *ConstantNumber::mul(ConstantNumber *rhs) {
  if (type == BasicType::I32)
    return new ConstantNumber(model::IntNumber(intValue() * rhs->intValue()));
  else if (type == BasicType::FLOAT)
    return new ConstantNumber(
        model::FloatNumber(floatValue() * rhs->floatValue()));
  else
    throw std::runtime_error("Unsupported type");
}

ConstantNumber *ConstantNumber::div(ConstantNumber *rhs) {
  if (type == BasicType::I32)
    return new ConstantNumber(model::IntNumber(intValue() / rhs->intValue()));
  else if (type == BasicType::FLOAT)
    return new ConstantNumber(
        model::FloatNumber(floatValue() / rhs->floatValue()));
  else
    throw std::runtime_error("Unsupported type");
}

ConstantNumber *ConstantNumber::rem(ConstantNumber *rhs) {
  if (type == BasicType::I32)
    return new ConstantNumber(model::IntNumber(intValue() % rhs->intValue()));
  else
    throw std::runtime_error("Unsupported type");
}

ConstantNumber *ConstantNumber::exor(ConstantNumber *rhs) {
  if (type == BasicType::I32)
    return new ConstantNumber(model::IntNumber(intValue() ^ rhs->intValue()));
  else if (type == BasicType::I1)
    return new ConstantNumber((intValue() ^ rhs->intValue()) != 0);
  else
    throw std::runtime_error("Unsupported type");
}

ConstantNumber *ConstantNumber::neg() {
  if (type == BasicType::I32)
    return new ConstantNumber(model::IntNumber(-intValue()));
  else if (type == BasicType::FLOAT)
    return new ConstantNumber(model::FloatNumber(-floatValue()));
  else
    throw std::runtime_error("Unsupported type");
}

ConstantNumber *ConstantNumber::lnot(ConstantNumber *rhs) {
  if (type == BasicType::I32 || type == BasicType::I1)
    return new ConstantNumber(intValue() == 0);
  else if (type == BasicType::FLOAT)
    return new ConstantNumber(floatValue() == 0.0f);
  else
    throw std::runtime_error("Unsupported type");
}

ConstantNumber *ConstantNumber::eq(ConstantNumber *rhs) {
  if (type == BasicType::I32 || type == BasicType::I1)
    return new ConstantNumber(intValue() == rhs->intValue());
  else if (type == BasicType::FLOAT)
    return new ConstantNumber(floatValue() == rhs->floatValue());
  else
    throw std::runtime_error("Unsupported type");
}

ConstantNumber *ConstantNumber::ne(ConstantNumber *rhs) {
  if (type == BasicType::I32 || type == BasicType::I1)
    return new ConstantNumber(intValue() != rhs->intValue());
  else if (type == BasicType::FLOAT)
    return new ConstantNumber(floatValue() != rhs->floatValue());
  else
    throw std::runtime_error("Unsupported type");
}

ConstantNumber *ConstantNumber::gt(ConstantNumber *rhs) {
  if (type == BasicType::I32 || type == BasicType::I1)
    return new ConstantNumber(intValue() > rhs->intValue());
  else if (type == BasicType::FLOAT)
    return new ConstantNumber(floatValue() > rhs->floatValue());
  else
    throw std::runtime_error("Unsupported type");
}

ConstantNumber *ConstantNumber::ge(ConstantNumber *rhs) {
  if (type == BasicType::I32 || type == BasicType::I1)
    return new ConstantNumber(intValue() >= rhs->intValue());
  else if (type == BasicType::FLOAT)
    return new ConstantNumber(floatValue() >= rhs->floatValue());
  else
    throw std::runtime_error("Unsupported type");
}

ConstantNumber *ConstantNumber::lt(ConstantNumber *rhs) {
  if (type == BasicType::I32 || type == BasicType::I1)
    return new ConstantNumber(intValue() < rhs->intValue());
  else if (type == BasicType::FLOAT)
    return new ConstantNumber(floatValue() < rhs->floatValue());
  else
    throw std::runtime_error("Unsupported type");
}

ConstantNumber *ConstantNumber::le(ConstantNumber *rhs) {
  if (type == BasicType::I32 || type == BasicType::I1)
    return new ConstantNumber(intValue() <= rhs->intValue());
  else if (type == BasicType::FLOAT)
    return new ConstantNumber(floatValue() <= rhs->floatValue());
  else
    throw std::runtime_error("Unsupported type");
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
