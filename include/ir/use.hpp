#include <memory>

namespace ir {

class User;
class Value;

class Use {
private:
  std::weak_ptr<User> user;
  std::weak_ptr<Value> value;

public:
  Use(std::weak_ptr<User> user, std::weak_ptr<Value> value)
      : user(user), value(value) {}
  Use(std::shared_ptr<User> user, std::shared_ptr<Value> value)
      : user(std::weak_ptr<User>(user)), value(std::weak_ptr<Value>(value)){};
  std::weak_ptr<User> getUser() { return user; }
  std::weak_ptr<Value> getValue() { return value; }
  void setUser(std::weak_ptr<User> user) { this->user = user; }
  void setValue(std::weak_ptr<Value> value) { this->value = value; }
  void setUser(std::shared_ptr<User> user) {
    this->user = std::weak_ptr<User>(user);
  }
  void setValue(std::shared_ptr<Value> value) {
    this->value = std::weak_ptr<Value>(value);
  }
};

} // namespace ir