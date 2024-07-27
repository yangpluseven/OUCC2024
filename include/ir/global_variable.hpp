#ifndef GLOBAL_VARIABLE_HPP
#define GLOBAL_VARIABLE_HPP

#include "constant.hpp"
#include <string>
#include <vector>

namespace ir {

class GlobalVariable : public Value {
private:
  std::string _name;
  bool _isConst;
  Constant *value;

public:
  GlobalVariable(bool isConst, Type *type, std::string name, Constant *value);

  bool isConst() const;

  std::vector<int> getDimensions() const;

  bool isSingle() const;

  bool isInBss() const;

  float getFloat() const;
  float getFloat(int index) const;

  int getInt() const;
  int getInt(int index) const;

  Constant *getValue() const;

  std::string getName() const override;
  std::string getRawName() const;

  std::string toString() const;
};

} // namespace ir

#endif // GLOBAL_VARIABLE_HPP
