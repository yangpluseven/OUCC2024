#ifndef CONSTANT_HPP
#define CONSTANT_HPP

#include "model/number.hpp"
#include "value.hpp"
#include <stdexcept>
#include <variant>
#include <vector>

namespace ir {

class Constant : public User {
public:
  Constant(Type *type);

  virtual ~Constant();

  virtual std::string toString() const = 0;
  virtual std::string getName() const = 0;
};

class ConstantNumber : public Constant {
private:
  model::Number _value;

  static BasicType *determineType(const model::Number &num);

public:
  explicit ConstantNumber(bool value);
  ConstantNumber(const model::Number &num);
  ConstantNumber(const ConstantNumber &other);
  ConstantNumber(ConstantNumber &&other) noexcept;

  model::Number getValue() const;
  float floatValue() const;
  int intValue() const;

  std::string getName() const override;
  ConstantNumber operator+(const ConstantNumber &rhs);
  ConstantNumber operator-(const ConstantNumber &rhs);
  ConstantNumber operator*(const ConstantNumber &rhs);
  ConstantNumber operator/(const ConstantNumber &rhs);
  ConstantNumber operator%(const ConstantNumber &rhs);
  ConstantNumber operator^(const ConstantNumber &rhs);
  ConstantNumber operator-();
  ConstantNumber operator!();
  ConstantNumber operator==(const ConstantNumber &rhs);
  ConstantNumber operator!=(const ConstantNumber &rhs);
  ConstantNumber operator>(const ConstantNumber &rhs);
  ConstantNumber operator>=(const ConstantNumber &rhs);
  ConstantNumber operator<(const ConstantNumber &rhs);
  ConstantNumber operator<=(const ConstantNumber &rhs);

  ConstantNumber *add(ConstantNumber *rhs);
  ConstantNumber *sub(ConstantNumber *rhs);
  ConstantNumber *mul(ConstantNumber *rhs);
  ConstantNumber *div(ConstantNumber *rhs);
  ConstantNumber *rem(ConstantNumber *rhs);
  ConstantNumber *exor(ConstantNumber *rhs);
  ConstantNumber *neg();
  ConstantNumber *lnot(ConstantNumber *rhs);
  ConstantNumber *eq(ConstantNumber *rhs);
  ConstantNumber *ne(ConstantNumber *rhs);
  ConstantNumber *gt(ConstantNumber *rhs);
  ConstantNumber *ge(ConstantNumber *rhs);
  ConstantNumber *lt(ConstantNumber *rhs);
  ConstantNumber *le(ConstantNumber *rhs);

  std::string toString() const override;
};

class ConstantZero : public Constant {
public:
  explicit ConstantZero(Type *type);

  std::string getName() const override;
  std::string toString() const override;
};

class ConstantArray : public Constant {
private:
  std::vector<Constant *> values;

public:
  ConstantArray(Type *type, std::vector<Constant *> values);

  std::vector<Constant *> &getValues();

  std::string toString() const override;
  std::string getName() const override;
};

} // namespace ir

#endif // CONSTANT_HPP
