#ifndef MODEL_NUMBER_HPP
#define MODEL_NUMBER_HPP

namespace model {

class Number {
public:
  virtual int intValue() const = 0;
  virtual float floatValue() const = 0;
};

class IntNumber : public Number {
private:
  int _value;

public:
  IntNumber(int value) : _value(value) {}
  int intValue() const override { return _value; }
  float floatValue() const override { return static_cast<float>(_value); }
};

class FloatNumber : public Number {
private:
  float _value;

public:
  FloatNumber(float value) : _value(value) {}
  int intValue() const override { return static_cast<int>(_value); }
  float floatValue() const override { return _value; }
};
} // namespace model

#endif