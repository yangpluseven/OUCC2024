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
    auto bo = BranchOpti(_module);
    auto cp = ConstProp(_module);
    auto dce = DeadCodeElim(_module);
    auto mp = MemoryProm(_module);
    auto rp = ReducePhi(_module);
    auto cse = CommonExpElim(_module);
    auto fi = FunctionInline(_module);

    bool l3;
    do {
      l3 = false;

      for (auto *function : _module->getFunctions()) {
        if (function->isDeclare())
          continue;
        std::vector shouldRun = {true, true, true, true, true, true};

        bool l0;
        do {
          l0 = false;

          bool l1;
          do {
            l1 = false;
            if ((_activated.at("-bo") == "1" || _activated.at("-O1") == "1") &&
                shouldRun[0]) {
              l1 |= onFunction(function, bo, shouldRun, 0);
            }
            if ((_activated.at("-cp") == "1" || _activated.at("-O1") == "1") &&
                shouldRun[1]) {
              l1 |= onFunction(function, cp, shouldRun, 1);
            }
            if ((_activated.at("-dce") == "1" || _activated.at("-O1") == "1") &&
                shouldRun[2]) {
              l1 |= onFunction(function, dce, shouldRun, 2);
            }
          } while (l1);

          if (_activated.at("-mp") == "1" || _activated.at("-O1") == "1") {
            l0 |= onFunction(function, mp, shouldRun, 3);
          }
          if (_activated.at("-rp") == "1" || _activated.at("-O1") == "1") {
            l0 |= onFunction(function, rp, shouldRun, 4);
          }
          if (_activated.at("-cse") == "1") {
            l0 |= onFunction(function, cse, shouldRun, 5);
          }
        } while (l0);
      }

      bool l2;
      do {
        l2 = false;
        if (_activated.at("-fi") == "1" || _activated.at("-inline") == "1") {
          l2 |= FunctionInline(_module).onModule();
        }
        l3 |= l2;
      } while (l2);
    } while (l3);
  }

  static bool onFunction(ir::Function *function, FunctionPass &pass,
                         std::vector<bool> &shouldRun, int id) {
    if (!shouldRun[id]) {
      return false;
    }
    bool changed = pass.onFunction(function);
    if (changed) {
      for (auto &&i : shouldRun) {
        i = true;
      }
    }
    else {
      shouldRun[id] = false;
    }
    return changed;
  }
};

} // namespace pass

#endif // PASS_MANAGER_HPP