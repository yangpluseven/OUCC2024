#include "include/ir/module.hpp"
#include "include/parser/SysYLexer.h"
#include "include/parser/SysYParser.h"
#include "include/parser/ast_visitor.hpp"
#include "runtime/antlr4-cpp-runtime-4.13.1/runtime/src/antlr4-runtime.h"
#include <bits/stdc++.h>
using namespace std;
using namespace parser;
using namespace antlr4;
using namespace ir;

void emitLLVM(const string &filename, Module *module) {
  ofstream writer(filename);
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
       [](const Function *func1, const Function *func2) {
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

string a =
    "int addFunc(int a, int b) {"
    "  return a + b;"
    "}"
    "int minusFunc(int a, int b) {"
    "  return a - b;"
    "}"
    "int main() {"
    "  int a;"
    "  return addFunc(2, 1);"
    "}";
string b =
    "int minusFunc(int a, int b) {"
    "  return a - b;"
    "}"
    "int main() {"
    "  return minusFunc(2, 1);"
    "}";
string c =
    "int main() {"
    "  int a;"
    "  return 0;"
    "}";

int main() {
  ANTLRInputStream input(a);
  SysYLexer lexer(&input);
  CommonTokenStream tokens(&lexer);
  SysYParser parser(&tokens);
  SysYParser::RootContext *root = parser.root();
  ASTVisitor visitor(root);
  Module *module = visitor.getModule();
  emitLLVM("test.ll", module);
  return 0;
}