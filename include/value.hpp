#ifndef IR_VALUE_HPP
#define IR_VALUE_HPP

#include "type.hpp"
#include <memory>
#include <unordered_set>
#include <vector>

namespace ir {

class User;
class Value;

class Use {
private:
  User *user;
  Value *value;

public:
  Use(User *user, Value *value);
  ~Use() = default;
  [[nodiscard]] User *getUser() const;
  [[nodiscard]] Value *getValue() const;
  void setUser(User *user);
  void setValue(Value *value);
};

class Value {
private:
  std::unordered_set<Use *> _used;

protected:
  Type *const type;

public:
  explicit Value(Type *type);
  virtual ~Value() = default;
  Type *getType() const;
  size_t getSize() const;
  void addUsed(Use *use);
  void replaceAllUseAs(Value *value);
  std::unordered_set<Use *> &getUsed();
  virtual std::string getName() const = 0;
};

class User : public Value {
protected:
  std::vector<Use *> uses;

public:
  explicit User(Type *type);
  ~User() override = default; // TODO: Implement destructor
  size_t size() const;
  void insert(Use *use);
  void insert(int index, Use *use);
  Use *erase(int index);
  Use *erase(const Value *value);
  Use *get(int index) const;
  void set(int index, Use *use);
  const std::vector<Use *> &getUses() const { return uses; }

  template <typename T,
            std::enable_if_t<std::is_base_of_v<Value, T>> * = nullptr>
  T *getOperand(int index) const {
    return static_cast<T *>(get(index)->getValue());
  }

  template <typename T,
            std::enable_if_t<std::is_base_of_v<Value, T>> * = nullptr>
  T *getLastOperand() const {
    return getOperand<T>(size() - 1);
  }

  bool empty() const;
};

} // namespace ir

#endif // IR_VALUE_HPP