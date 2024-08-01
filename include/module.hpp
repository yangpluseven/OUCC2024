#ifndef IR_MODULE_HPP
#define IR_MODULE_HPP

#include "global_variable.hpp"
#include "function.hpp"
#include <unordered_map>
#include <vector>

namespace ir {

class Module {
private:
  std::unordered_map<std::string, GlobalVariable *> globals;
  std::unordered_map<std::string, Function *> functions;

public:
  Module() = default;

  void addGlobal(GlobalVariable *global);
  void addFunction(Function *function);

  bool hasGlobal() const;
  bool hasFunction() const;

  GlobalVariable *getGlobal(const std::string &name);
  Function *getFunction(const std::string &name);

  std::vector<GlobalVariable *> getGlobals() const;
  std::vector<Function *> getFunctions() const;
};

} // namespace ir

#endif // IR_MODULE_HPP
