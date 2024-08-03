#ifndef PASS_MANAGER_HPP
#define PASS_MANAGER_HPP

#include "module.hpp"
#include "pass.hpp"

namespace pass {
class PassManager {
private:
  const ir::Module *module;

public:
  explicit PassManager(const ir::Module *module) : module(module) {
  }

  void run() const {
    bool modified;
    do {
      modified = false;
      modified |= BranchOptPass(module).run();
      // modified |= PromotePass(module).run();
      modified |= ConstPropPass(module).run();
      modified |= DCEPass(module).run();
    } while (modified);
  }
};
}

#endif //PASS_MANAGER_HPP