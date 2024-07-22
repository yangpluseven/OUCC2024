#include "parser/ast_visitor.hpp"

namespace parser {

void ASTVisitor::initBuiltInFuncs() {
  _module->addFunction(_symbolTable->makeFunc(ir::BasicType::I32, "getint"));
  _module->addFunction(_symbolTable->makeFunc(ir::BasicType::I32, "getch"));
  _module->addFunction(_symbolTable->makeFunc(ir::BasicType::I32, "getarray")
                           ->addArg(new ir::Argument(
                               new ir::PointerType(ir::BasicType::I32), "a")));
  _module->addFunction(
      _symbolTable->makeFunc(ir::BasicType::FLOAT, "getfloat"));
  _module->addFunction(
      _symbolTable->makeFunc(ir::BasicType::I32, "getfarray")
          ->addArg(new ir::Argument(new ir::PointerType(ir::BasicType::FLOAT),
                                    "a")));
  _module->addFunction(_symbolTable->makeFunc(ir::BasicType::VOID, "putint")
                           ->addArg(new ir::Argument(ir::BasicType::I32, "a")));
  _module->addFunction(_symbolTable->makeFunc(ir::BasicType::VOID, "putch")
                           ->addArg(new ir::Argument(ir::BasicType::I32, "a")));
  _module->addFunction(_symbolTable->makeFunc(ir::BasicType::VOID, "putarray")
                           ->addArg(new ir::Argument(ir::BasicType::I32, "n"))
                           ->addArg(new ir::Argument(ir::BasicType::I32, "a")));
  _module->addFunction(
      _symbolTable->makeFunc(ir::BasicType::VOID, "putfloat")
          ->addArg(new ir::Argument(ir::BasicType::FLOAT, "a")));
  _module->addFunction(
      _symbolTable->makeFunc(ir::BasicType::VOID, "putfarray")
          ->addArg(new ir::Argument(ir::BasicType::I32, "n"))
          ->addArg(new ir::Argument(ir::BasicType::FLOAT, "a")));
  _module->addFunction(
      _symbolTable->makeFunc(ir::BasicType::VOID, "_sysy_starttime")
          ->addArg(new ir::Argument(ir::BasicType::I32, "lineno")));
  _module->addFunction(
      _symbolTable->makeFunc(ir::BasicType::VOID, "_sysy_stoptime")
          ->addArg(new ir::Argument(ir::BasicType::I32, "lineno")));
}

void ASTVisitor::initSysCalls() {
  _module->addFunction(
      _symbolTable->makeFunc(ir::BasicType::VOID, "memset")
          ->addArg(
              new ir::Argument(new ir::PointerType(ir::BasicType::I32), "addr"))
          ->addArg(new ir::Argument(ir::BasicType::I32, "value"))
          ->addArg(new ir::Argument(ir::BasicType::I32, "size")));
}

void ASTVisitor::checkIfIsProcessed() {
  if (_isProcessed)
    return;
  _isProcessed = true;
  visitRoot(_rootAST);
  formatLLVM();
}

void ASTVisitor::formatLLVM() {
  for (auto func : _module->getFunctions()) {
    if (func->isDeclare())
      continue;
    for (int i = 0; i < func->size(); i++) {
      auto block = func->get(i);
      for (int j = 0; j < block->size() - 1; j++) {
        auto terminal = block->get(i);
        if (dynamic_cast<ir::BranchInst *>(terminal) ||
            dynamic_cast<ir::RetInst *>(terminal)) {
          while (block->size() > j + 1)
            block->remove(j);
          break;
        }
      }
      if (block->isEmpty() ||
          !(dynamic_cast<ir::RetInst *>(block->getLast()) ||
            dynamic_cast<ir::BranchInst *>(block->getLast())))
        block->add(new ir::BranchInst(block, func->get(i + 1)));
    }
  }
}

void ASTVisitor::allocInitVal(
    std::vector<int> &dimensions,
    std::map<int, SysYParser::AdditiveExpContext *> &exps, int base,
    SysYParser::InitValContext *src) {
  int offset = 0;
  for (auto exp : src->initVal()) {
    if (exp->additiveExp() == nullptr) {
      int size = 1;
      for (auto i = 1; i < dimensions.size(); i++)
        size *= dimensions[i];
      offset = (offset + size - 1) / size * size;
      std::vector<int> newDimensions(dimensions.begin() + 1, dimensions.end());
      allocInitVal(newDimensions, exps, base + offset, exp);
      offset += size;
    } else {
      exps[base + offset] = exp->additiveExp();
      offset++;
    }
  }
}

ASTVisitor::ASTVisitor(SysYParser::RootContext *rootAST) : _rootAST(rootAST) {
  _symbolTable->in();
  initBuiltInFuncs();
  initSysCalls();
}

ir::Module *ASTVisitor::getModule() {
  checkIfIsProcessed();
  return _module;
}

std::any ASTVisitor::visitType(SysYParser::TypeContext *ctx) {
  if (ctx->getText() == "int")
    return ir::BasicType::I32;
  else if (ctx->getText() == "float")
    return ir::BasicType::FLOAT;
  else if (ctx->getText() == "void")
    return ir::BasicType::VOID;
  else
    throw std::runtime_error("Unsupported type: " + ctx->getText());
}

std::any ASTVisitor::visitDimensions(SysYParser ::DimensionsContext *ctx) {
  auto dimensions = std::vector<int>();
  for (auto exp : ctx->additiveExp()) {
    dimensions.push_back(
        std::any_cast<ir::ConstantNumber *>(visitAdditiveExp(exp))->intValue());
  }
  return dimensions;
}

std::any ASTVisitor::visitVarDecl(SysYParser::VarDeclContext *ctx) {
  _isConst = ctx->CONST() != nullptr;
  return SysYBaseVisitor::visitVarDecl(ctx);
}

std::any ASTVisitor::visitArrayVarDef(SysYParser::ArrayVarDefContext *ctx) {
  auto name = ctx->Ident()->getSymbol()->getText();
  auto dimensions =
      std::any_cast<std::vector<int>>(visitDimensions(ctx->dimensions()));
  auto initVal = ctx->initVal();
  if (_isConst || _symbolTable->size() == 1) {
    std::map<int, SysYParser::AdditiveExpContext *> exps;
    if (initVal != nullptr)
      allocInitVal(dimensions, exps, 0, initVal);
    auto reverseDimensions = dimensions;
    reverseDimensions.reserve(dimensions.size());
    // WIP
  }
}

} // namespace parser