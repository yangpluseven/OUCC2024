#include <memory>

namespace ir {
class Type {
public:
  virtual ~Type() {}
  virtual int getSize() = 0;
  virtual Type* baseType() = 0;
};
} // namespace ir