#ifndef COMPILER_HPP
#define COMPILER_HPP

#include "mir_gen.hpp"
#include "ast_visitor.hpp"
#include <string>
#include <unordered_map>
#include <utility>

class Compiler {
private:
  static ir::Module *_module;
  static std::unordered_map<std::string, mir::MachineFunction *> _funcs;

public:
  static std::unordered_map<std::string, std::string> optionValues;

  Compiler() = default;

  static void compile();
  static void emitLLVM();
  static void emitMIR();
};

class CodeGenerator {
private:
  std::unordered_set<ir::GlobalVariable *> _globals;
  std::unordered_map<std::string, mir::MachineFunction *> _funcs;

  void buildGlobals(std::ostringstream &builder) const;
  void buildFuncs(std::ostringstream &builder) const;

public:
  CodeGenerator(std::unordered_set<ir::GlobalVariable *> globals,
                std::unordered_map<std::string, mir::MachineFunction *> funcs)
      : _globals(std::move(globals)), _funcs(std::move(funcs)) {}

  std::string getOutPut() const {
    std::ostringstream builder;
    buildGlobals(builder);
    buildFuncs(builder);
    return std::move(builder.str());
  }
};

#endif // COMPILER_HPP