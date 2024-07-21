#ifndef IR_MODULE_HPP
#define IR_MODULE_HPP

#include "function.hpp"
#include "global_variable.hpp"
#include <unordered_map>

namespace ir {
class Module {
private:
  std::unordered_map<std::string, GlobalVariable *> globals;
  std::unordered_map<std::string, Function *> functions;

public:
  Module() = default;
  
  void addGlobal(GlobalVariable *global) {
    globals[global->getName()] = global;
  }

  void addFunction(Function *function) {
    functions[function->getName()] = function;
  }

  bool hasGlobal() const { return !globals.empty(); }

  bool hasFunction() const { return !functions.empty(); }

  GlobalVariable *getGlobal(const std::string &name) {
    if (globals.find(name) != globals.end()) {
      return globals[name];
    } else
      return nullptr;
  }

  Function *getFunction(const std::string &name) {
    if (functions.find(name) != functions.end()) {
      return functions[name];
    } else
      return nullptr;
  }

  std::vector<GlobalVariable *> getGlobals() {
    std::vector<GlobalVariable *> result;
    for (auto &global : globals) {
      result.push_back(global.second);
    }
    return result;
  }

  std::vector<Function *> getFunctions() {
    std::vector<Function *> result;
    for (auto &function : functions) {
      result.push_back(function.second);
    }
    return result;
  }
};
} // namespace ir

#endif