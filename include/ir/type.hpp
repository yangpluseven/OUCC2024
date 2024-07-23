#ifndef IR_TYPE_HPP
#define IR_TYPE_HPP

#include <string>
#include <vector>

namespace ir {

class Type {
public:
  Type() = default;
  virtual ~Type();

  virtual Type *baseType() const = 0;
  virtual size_t getSize() const = 0;
  virtual std::string toString() const = 0;

  static Type *CheckEquality(Type *lhs, Type *rhs);

  bool operator==(const Type &rhs) const;
};

class BasicType : public Type {
public:
  static BasicType *const I1;
  static BasicType *const I32;
  static BasicType *const FLOAT;
  static BasicType *const VOID;

  size_t getSize() const override;
  std::string toString() const override;
  Type *baseType() const override;

private:
  size_t _size;
  std::string _name;

  BasicType(size_t size, std::string name);
};

class PointerType : public Type {
private:
  Type *const _bType;

public:
  PointerType(Type *baseType);

  Type *baseType() const override;
  size_t getSize() const override;
  std::string toString() const override;
};

class ArrayType : public Type {
private:
  Type *const _bType;
  size_t _arraySize;

public:
  ArrayType(Type *baseType, size_t arraySize);

  Type *baseType() const override;
  size_t getArraySize();
  size_t getSize() const override;
  Type *getScalarType();
  std::vector<ArrayType *> &getArrayTypes();
  std::vector<size_t> &getArraySizes();
  std::string toString() const override;
};

} // namespace ir

#endif // IR_TYPE_HPP
