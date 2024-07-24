#ifndef MODEL_NUMBER_HPP
#define MODEL_NUMBER_HPP

#include <variant>

namespace model {

class Number {
private:
  std::variant<int, float> _value;

public:
  int intValue() const {
    if (std::holds_alternative<int>(_value))
      return std::get<int>(_value);
    else
      return static_cast<int>(std::get<float>(_value));
  }

  float floatValue() const {
    if (std::holds_alternative<int>(_value))
      return static_cast<float>(std::get<int>(_value));
    else
      return std::get<float>(_value);
  }

  std::variant<int, float> getValue() const { return _value; }

  Number(int value) : _value(value) {}
  Number(float value) : _value(value) {}
  Number(const Number &other) : _value(other._value) {}
  Number(Number &&other) : _value(std::move(other._value)) {}
  Number &operator=(const Number &other) {
    _value = other._value;
    return *this;
  }
  Number &operator=(Number &&other) {
    _value = std::move(other._value);
    return *this;
  }
};
} // namespace model

#endif