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
  ~Use() = default; // TODO: Implement destructor
  [[nodiscard]] User *getUser() const;
  [[nodiscard]] Value *getValue() const;
  void setUser(User *user);
  void setValue(Value *value);
};

class Value {
private:
  std::unordered_set<Use *> _uses;

protected:
  Type *const type;

public:
  explicit Value(Type *type);
  virtual ~Value() = default; // TODO: Implement destructor
  Type *getType() const;
  size_t getSize() const;
  void addUse(Use *use);
  void replaceAllUseAs(Value *value);
  std::unordered_set<Use *> &getUses();
  virtual std::string getName() const = 0;
};

class User : public Value {
protected:
  std::vector<Use *> operands;

public:
  explicit User(Type *type);
  ~User() override = default; // TODO: Implement destructor
  size_t size() const;
  void add(Use *use);
  void add(int index, Use *use);
  Use *remove(int index);
  Use *remove(const Value *value);
  Use *get(int index) const;

  template <typename T, std::enable_if_t<
              std::is_base_of_v<Value, T>> * = nullptr>
  T *getOperand(int index) const {
    return static_cast<T *>(get(index)->getValue());
  }

  template <typename T, std::enable_if_t<
              std::is_base_of_v<Value, T>> * = nullptr>
  T *getLastOperand() const {
    return getOperand<T>(size() - 1);
  }

  bool isEmpty() const;
  void set(int index, Use *use);
};

} // namespace ir

#endif // IR_VALUE_HPP