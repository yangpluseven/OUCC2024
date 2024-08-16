#ifndef PASS_MANAGER_HPP
#define PASS_MANAGER_HPP

#include "module.hpp"
#include "pass.hpp"

namespace pass {
class PassManager {
private:
  const ir::Module *_module;
  const std::unordered_map<std::string, std::string> &_activated;

public:
  explicit
  PassManager(const ir::Module *module,
              const std::unordered_map<std::string, std::string> &activated)
      : _module(module), _activated(activated) {}

  void run() const {
    bool l2;
    do {
      bool l0;
      do {
        l0 = false;
        bool l1;
        do {
          l1 = false;
          if (_activated.at("-O1") == "1")
            l1 |= BranchOpt(_module).onModule();
          if (_activated.at("-O1") == "1")
            l1 |= ConstProp(_module).onModule();
          if (_activated.at("-O1") == "1")
            l1 |= DeadCodeEli(_module).onModule();
        } while (l1);
        if (_activated.at("-O1") == "1")
          l0 |= MemoryPromote(_module).onModule();
      } while (l0);
      l2 = false;
      bool l3;
      do {
        l3 = false;
        if (_activated.at("-inline") == "1")
          l3 |= FunctionInline(_module).onModule();
        if (_activated.at("-O1") == "1")
          l3 |= ReducePhi(_module).onModule();
        l2 |= l3;
      } while (l3);
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