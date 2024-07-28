#ifndef IR_CONSTANT_HPP
#define IR_CONSTANT_HPP

#include "model/number.hpp"
#include "value.hpp"
#include <stdexcept>
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
  explicit ConstantNumber(const model::Number &num);
  ConstantNumber(const ConstantNumber &other);
  ConstantNumber(ConstantNumber &&other) noexcept;

  model::Number getValue() const;
  float floatValue() const;
  int intValue() const;

  std::string getName() const override;
  ConstantNumber operator+(const ConstantNumber &rhs) const ;
  ConstantNumber operator-(const ConstantNumber &rhs) const ;
  ConstantNumber operator*(const ConstantNumber &rhs) const ;
  ConstantNumber operator/(const ConstantNumber &rhs) const ;
  ConstantNumber operator%(const ConstantNumber &rhs) const ;
  ConstantNumber operator^(const ConstantNumber &rhs) const ;
  ConstantNumber operator-() const;
  ConstantNumber operator!() const;
  ConstantNumber operator==(const ConstantNumber &rhs) const ;
  ConstantNumber operator!=(const ConstantNumber &rhs) const ;
  ConstantNumber operator>(const ConstantNumber &rhs) const ;
  ConstantNumber operator>=(const ConstantNumber &rhs) const ;
  ConstantNumber operator<(const ConstantNumber &rhs) const ;
  ConstantNumber operator<=(const ConstantNumber &rhs) const ;

  ConstantNumber *add(const ConstantNumber *rhs) const ;
  ConstantNumber *sub(const ConstantNumber *rhs) const ;
  ConstantNumber *mul(const ConstantNumber *rhs) const ;
  ConstantNumber *div(const ConstantNumber *rhs) const ;
  ConstantNumber *rem(const ConstantNumber *rhs) const ;
  ConstantNumber *exor(const ConstantNumber *rhs) const ;
  ConstantNumber *neg() const;
  ConstantNumber *lnot() const;
  ConstantNumber *eq(const ConstantNumber *rhs) const ;
  ConstantNumber *ne(const ConstantNumber *rhs) const ;
  ConstantNumber *gt(const ConstantNumber *rhs) const ;
  ConstantNumber *ge(const ConstantNumber *rhs) const ;
  ConstantNumber *lt(const ConstantNumber *rhs) const ;
  ConstantNumber *le(const ConstantNumber *rhs) const ;

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

#endif // IR_CONSTANT_HPP
