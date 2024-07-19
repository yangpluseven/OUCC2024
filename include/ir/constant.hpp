#ifndef CONSTANT_HPP
#define CONSTANT_HPP

#include "user.hpp"

namespace ir {

class Constant : public User {
public:
  Constant(Type *type) : User(type) {}

  virtual ~Constant() {}

  virtual std::string toString() = 0;

  virtual std::string &getName() = 0;
};

} // namespace ir

#endif