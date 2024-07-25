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

any test1() {
  return BasicType::I32;
}

any test2() {
  return make_any<Type *>(BasicType::I32);
}

int main() {
  ANTLRInputStream input("int addFunc(int a, int b) {"
                         "  return a + b;"
                         "}"
                         "int main() {"
                         "  return addFunc(1, 2);"
                         "}");
  SysYLexer lexer(&input);
  CommonTokenStream tokens(&lexer);
  SysYParser parser(&tokens);
  SysYParser::RootContext *root = parser.root();
  ASTVisitor visitor(root);
  Module *module = visitor.getModule();


  // BasicType *t1 = any_cast<BasicType *>(test1());
  // Type *tt1 = static_cast<Type *>(t1);
  // Type *t2 = any_cast<Type *>(test2());
  // cout << t2->toString() << endl;
  return 0;
}