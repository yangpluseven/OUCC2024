#ifndef COMPILER_HPP
#define COMPILER_HPP

#include "ast_visitor.hpp"
#include "mir_gen.hpp"
#include <string>
#include <unordered_map>
#include <utility>

class Compiler {
private:
  static ir::Module *_module;
  static std::unordered_set<ir::GlobalVariable *> _globals;
  static std::unordered_map<std::string, mir::MachineFunction *> _funcs;

public:
  static std::unordered_map<std::string, std::string> optionValues;

  Compiler() = default;

  static void compile();
  static void emitLLVM();
  static void emitMIR();
  static void emitAssemble();
  static void writeGlobals();
  static void writeFuncs();
  static void setModule(ir::Module *module) { _module = module; }
};

#endif // COMPILER_HPP