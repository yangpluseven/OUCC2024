#ifndef PASS_MANAGER_HPP
#define PASS_MANAGER_HPP
#include "pass.hpp"
#include "ir/module.hpp"

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
      modified |= (new BranchOptPass(module))->run();
      modified |= (new PromotePass(module))->run();
      modified |= (new ConstPropPass(module))->run();
      modified |= (new DCEPass(module))->run();
    } while (modified);
  }
};
}

#endif //PASS_MANAGER_HPP