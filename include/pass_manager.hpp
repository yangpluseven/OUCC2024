#ifndef PASS_MANAGER_HPP
#define PASS_MANAGER_HPP

#include "module.hpp"
#include "pass.hpp"

namespace pass {
class PassManager {
private:
  const ir::Module *module;

public:
  explicit PassManager(const ir::Module *module) : module(module) {}

  void run() const {
    bool modified;
    do {
      modified = false;
      bool inner;
      do {
        inner = false;
        inner |= BranchOptPass(module).run();
        inner |= ConstPropPass(module).run();
        inner |= DCEPass(module).run();
        modified |= inner;
      } while (inner);
      modified |= PromotePass(module).run();
      modified |= DCEPass(module).run();
    } while (modified);
  }
};
} // namespace pass

#endif // PASS_MANAGER_HPP