#ifndef COMPILER_HPP
#define COMPILER_HPP

#include "parser/ast_visitor.hpp"
#include <string>
#include <unordered_map>


class Compiler {
private:
  static ir::Module *module;

public:
  static std::unordered_map<std::string, std::string> optionValues;

  Compiler() = default;

  static void compile();

  static void emitLLVM();
};

#endif //COMPILER_HPP