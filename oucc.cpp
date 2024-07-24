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

int main() {
  ANTLRInputStream input("int main() { int a = 1; return a; }");
  SysYLexer lexer(&input);
  CommonTokenStream tokens(&lexer);
  SysYParser parser(&tokens);
  SysYParser::RootContext *root = parser.root();
  ASTVisitor visitor(root);
  Module *module = visitor.getModule();
  return 0;
}