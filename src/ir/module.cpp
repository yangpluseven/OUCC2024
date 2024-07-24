#include "ir/module.hpp"

namespace ir {

void Module::addGlobal(GlobalVariable *global) {
  globals[global->getName()] = global;
}

void Module::addFunction(Function *function) {
  functions[function->getName()] = function;
}

bool Module::hasGlobal() const { return !globals.empty(); }

bool Module::hasFunction() const { return !functions.empty(); }

GlobalVariable *Module::getGlobal(const std::string &name) {
  auto it = globals.find(name);
  return it != globals.end() ? it->second : nullptr;
}

Function *Module::getFunction(const std::string &name) {
  auto it = functions.find(name);
  return it != functions.end() ? it->second : nullptr;
}

std::vector<GlobalVariable *> Module::getGlobals() {
  std::vector<GlobalVariable *> result;
  for (auto &global : globals) {
    result.push_back(global.second);
  }
  return result;
}

std::vector<Function *> Module::getFunctions() {
  std::vector<Function *> result;
  for (auto &function : functions) {
    result.push_back(function.second);
  }
  return result;
}

} // namespace ir
