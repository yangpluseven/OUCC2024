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
  ~Use();
  User *getUser();
  Value *getValue();
  void setUser(User *user);
  void setValue(Value *value);
};

class Value {
private:
  std::unordered_set<Use *> uses;

protected:
  Type *const type;

public:
  Value(Type *type);
  virtual ~Value();
  Type *const getType();
  int getSize() const;
  void addUse(Use *use);
  void replaceAllUseAs(Value *value);
  std::unordered_set<Use *> &getUses();
  virtual std::string getName() const = 0;
};

class User : public Value {
protected:
  std::vector<Use *> operands;

public:
  User(Type *type);
  virtual ~User();
  size_t size() const;
  void add(Use *use);
  void add(int index, Use *use);
  Use *remove(int index);
  Use *remove(Value *value);
  Use *get(int index) const;

  template <typename T, typename std::enable_if<
              std::is_base_of<Value, T>::value>::type * = nullptr>
  T *getOperand(int index) const {
    return static_cast<T *>(get(index)->getValue());
  }

  template <typename T, typename std::enable_if<
              std::is_base_of<Value, T>::value>::type * = nullptr>
  T *getLastOperand() const {
    return getOperand<T>(size() - 1);
  }

  bool isEmpty() const;
  void set(int index, Use *use);
};

} // namespace ir

#endif // IR_VALUE_HPP