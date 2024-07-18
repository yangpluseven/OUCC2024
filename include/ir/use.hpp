#ifndef IR_USE_HPP
#define IR_USE_HPP
#include "user.hpp"
#include <memory>

namespace ir {

class Use {
private:
  User *user;
  Value *value;

public:
  Use(User *user, Value *value) : user(user), value(value) {}
  ~Use() {
    // TODO: Implement destructor
  }
  User *getUser() { return user; }
  Value *getValue() { return value; }
  void setUser(User *user) { this->user = user; }
  void setValue(Value *value) { this->value = value; }
};

} // namespace ir
#endif // IR_USE_HPP