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
    bool l2;
    do {
      bool l0;
      do {
        l0 = false;
        bool l1;
        do {
          l1 = false;
          l1 |= BranchOpt(module).onModule();
          l1 |= ConstProp(module).onModule();
          l1 |= DeadCodeEli(module).onModule();
        } while (l1);
        l0 |= MemoryPromote(module).onModule();
      } while (l0);
      l2 = false;
      while (FunctionInline(module).onModule()) {
        l2 = true;
      }
    } while (l2);
  }
};
} // namespace pass

#endif // PASS_MANAGER_HPP