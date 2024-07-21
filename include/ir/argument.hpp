#ifndef ARGUMENT_HPP
#define ARGUMENT_HPP
#include "value.hpp"
#include "type.hpp"
#include <string>

namespace ir {

class Argument : public Value {
private:
  const std::string name;

public:
  Argument(Type *type, std::string name) : Value(type), name(name) {}

  std::string getName() const override { return "%"+name; }

  std::string toString() const { return type->toString(); }
};

} // namespace ir

#endif // ARGUMENT_HPP