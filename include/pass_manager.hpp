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
      bool l3;
      do {
        l3 = false;
        l3 |= FunctionInline(module).onModule();
        l3 |= ReducePhi(module).onModule();
        l2 |= l3;
      } while(l3);
    } while (l2);

    // ReducePhi(module).onModule();
    // FunctionInline(module).onModule();
    // BranchOpt(module).onModule();
    // ConstProp(module).onModule();
    // DeadCodeEli(module).onModule();
    // MemoryPromote(module).onModule();

  }
};
} // namespace pass

#endif // PASS_MANAGER_HPP