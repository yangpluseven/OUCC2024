#include <memory>

namespace ir {
class Type {
public:
  virtual ~Type() {}
  virtual int getSize() = 0;
  virtual std::shared_ptr<Type> baseType() = 0;
};
} // namespace ir