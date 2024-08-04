#ifndef IR_TYPE_HPP
#define IR_TYPE_HPP

#include <string>
#include <vector>

namespace ir {

class Type {
public:
  Type() = default;
  virtual ~Type();

  [[nodiscard]] virtual Type *baseType() const = 0;
  [[nodiscard]] virtual size_t getSize() const = 0;
  [[nodiscard]] virtual std::string str() const = 0;

  static Type *checkEquality(Type *lhs, const Type *rhs);

  bool operator==(const Type &rhs) const;
};

class BasicType : public Type {
public:
  static BasicType *const I1;
  static BasicType *const I32;
  static BasicType *const FLOAT;
  static BasicType *const VOID;

  [[nodiscard]] size_t getSize() const override;
  [[nodiscard]] std::string str() const override;
  [[nodiscard]] Type *baseType() const override;

private:
  size_t _size;
  std::string _name;

  BasicType(size_t size, std::string name);
};

class PointerType : public Type {
private:
  Type *const _bType;

public:
  explicit PointerType(Type *baseType);

  [[nodiscard]] Type *baseType() const override;
  [[nodiscard]] size_t getSize() const override;
  [[nodiscard]] std::string str() const override;
};

class ArrayType : public Type {
private:
  Type *const _bType;
  size_t _arraySize;

public:
  ArrayType(Type *baseType, size_t arraySize);

  [[nodiscard]] Type *baseType() const override;
  [[nodiscard]] size_t getArraySize() const;
  [[nodiscard]] size_t getSize() const override;
  Type *getScalarType();
  std::vector<ArrayType *> getArrayTypes();
  std::vector<size_t> getArraySizes();
  [[nodiscard]] std::string str() const override;
};

} // namespace ir

#endif // IR_TYPE_HPP
