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
    bool outterChanged;
    do {
      outterChanged = false;
      bool innerChanged;
      do {
        innerChanged = false;
        innerChanged |= BranchOpt(module).onModule();
        innerChanged |= ConstProp(module).onModule();
        innerChanged |= DeadCodeEli(module).onModule();
      } while (innerChanged);
      outterChanged |= MemoryPromote(module).onModule();
      outterChanged |= DeadCodeEli(module).onModule();
    } while (outterChanged);
  }
};
} // namespace pass

#endif // PASS_MANAGER_HPP