#include "compiler.hpp"
#include "parser/SysYParser.h"
#include "parser/SysYLexer.h"
#include "ANTLRInputStream.h"
#include <fstream>
#include <sstream>

std::unordered_map<std::string, std::string> Compiler::optionValues{
    {"-S", "0"}, {"-O1", "0"}, {"-o", "out"}, {"", "main.sy"}, {"-dispf", "0"},
    {"-emit-llvm", "0"}, {"-emit-mir", "0"}};

ir::Module *Compiler::module;

void Compiler::compile() {
  const std::string filePath = optionValues[""];
  std::ifstream file(filePath);
  if (!file.is_open()) {
    throw std::runtime_error("File not found");
  }
  std::stringstream buffer;
  buffer << file.rdbuf();
  antlr4::ANTLRInputStream input(buffer.str());
  SysYLexer lexer(&input);
  antlr4::CommonTokenStream tokens(&lexer);
  SysYParser parser(&tokens);
  SysYParser::RootContext *root = parser.root();
  parser::ASTVisitor visitor(root);
  module = visitor.getModule();
  if (optionValues["-emit-llvm"] == "1") {
    emitLLVM();
    if (optionValues["-emit-mir"] == "1" || optionValues["-S"] == "1") {
      std::cout << "Emit llvm prior to mir and assembly. quitting." << std::endl;
      return;
    }
  }
  if (optionValues["-emit-mir"] == "1") {
    // TODO
  }
  if (optionValues["-S"] == "1") {
    // TODO
  }
}

void Compiler::emitLLVM() {
  const std::string filename = optionValues["-o"];
  std::ofstream writer(filename);
  if (!writer.is_open()) {
    throw std::runtime_error("Cannot open file: " + filename);
  }

  for (const auto &global : module->getGlobals()) {
    writer << global->toString() << '\n';
  }

  if (module->hasGlobal()) {
    writer << '\n';
  }

  auto functions = module->getFunctions();
  sort(functions.begin(), functions.end(),
       [](const ir::Function *func1, const ir::Function *func2) {
         if (func1->isDeclare() != func2->isDeclare()) {
           return func1->isDeclare() < func2->isDeclare();
         }
         return func1->getName() < func2->getName();
       });

  for (const auto &func : functions) {
    writer << func->toString() << '\n';
  }

  writer.close();
  if (writer.fail()) {
    throw std::runtime_error("Cannot write to file: " + filename);
  }
}