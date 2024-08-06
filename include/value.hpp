#ifndef IR_VALUE_HPP
#define IR_VALUE_HPP

#include "type.hpp"
#include <memory>
#include <unordered_map>
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
  std::unordered_set<Use *> _uses;

protected:
  Type *const type;

public:
  explicit Value(Type *type);
  virtual ~Value() = default;
  Type *getType() const;
  size_t getSize() const;
  void insertUse(Use *use);
  void replaceAllUseAs(Value *value);
  std::unordered_set<Use *> &getUses();
  virtual std::string getName() const { return ""; };
};

class User : public Value {
protected:
  std::vector<Use *> useOperands;

public:
  explicit User(Type *type);
  ~User() override = default;
  size_t size() const;
  void insert(Use *use);
  void insert(int index, Use *use);
  Use *erase(int index);
  Use *erase(const Value *value);
  Use *get(int index) const;
  void set(int index, Use *use);
  const std::vector<Use *> &getUseOperands() const { return useOperands; }
  void replaceOperandsFrom(const std::unordered_map<Value *, Value *> &replaceMap,
                       const User *other);

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

  using iterator = std::vector<Use *>::iterator;
  using const_iterator = std::vector<Use *>::const_iterator;

  iterator begin() { return useOperands.begin(); }
  iterator end() { return useOperands.end(); }
  const_iterator begin() const { return useOperands.begin(); }
  const_iterator end() const { return useOperands.end(); }
  const_iterator cbegin() const { return useOperands.cbegin(); }
  const_iterator cend() const { return useOperands.cend(); }
};

} // namespace ir

#endif // IR_VALUE_HPP