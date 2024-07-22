#ifndef PARSER_AST_VISITOR_HPP
#define PARSER_AST_VISITOR_HPP

#include "SysYBaseVisitor.h"
#include "ir/basic_block.hpp"
#include "ir/constant.hpp"
#include "ir/function.hpp"
#include "ir/instruction.hpp"
#include "ir/module.hpp"
#include "ir/type.hpp"
#include "ir/value.hpp"
#include "model/number.hpp"
#include <list>
#include <map>
#include <stdexcept>
#include <unordered_map>

class SymbolTable {
private:
  std::list<std::unordered_map<std::string, ir::Value *>> _table;

  ir::Value *get(const std::string &name) {
    for (auto item : _table) {
      if (item.find(name) != item.end())
        return item[name];
    }
    throw std::runtime_error("Undefined variable: " + name);
  }

  std::map<int, Number> submap(const std::map<int, Number> &values, int fromKey,
                               int toKey) {
    std::map<int, Number> result;
    for (auto i : values)
      if (i.first >= fromKey && i.first <= toKey)
        result[i.first] = i.second;
    return result;
  }

  ir::Constant *fuseConst(ir::Type *type, const std::map<int, Number> &values,
                          int base) {
    if (dynamic_cast<ir::BasicType *>(type))
      if (values.find(base) != values.end())
        return new ir::ConstantNumber(values.at(base));
      else
        return new ir::ConstantNumber(IntNumber(0));

    if (values.empty())
      return new ir ::ConstantZero(type);

    auto arrayType = dynamic_cast<ir::ArrayType *>(type);
    int size = 1;
    for (auto i : arrayType->getArraySizes())
      size *= i;

    std::vector<ir::Constant *> array;
    for (int i = base; i < base + size; i += size / arrayType->getArraySize())
      array.push_back(fuseConst(
          arrayType->baseType(),
          submap(values, i, i + size / arrayType->getArraySize()), i));
    return new ir::ConstantArray(arrayType, array);
  }

public:
  ir::Value *getData(const std::string &name) { return get(name); }

  ir::Function *getFunc(const std::string &name) {
    auto symbol = get(name);
    if (auto func = dynamic_cast<ir::Function *>(symbol))
      return func;
    throw std::runtime_error("Undefined function: " + name);
  }

  void in() { _table.push_front({}); }

  ir::Function *makeFunc(ir::Type *type, const std::string &name) {
    auto func = new ir::Function(type, name);
    _table.back()[name] = func;
    return func;
  }

  ir::GlobalVariable *makeGlobal(bool isConst, ir::Type *type,
                                 const std::string &name, Number value) {
    auto symbol = new ir::GlobalVariable(isConst, type, name,
                                         new ir::ConstantNumber(value));
    _table.front()[name] = symbol;
    return symbol;
  }

  ir::GlobalVariable *makeGlobal(bool isConst, ir::Type *type,
                                 const std::string &name,
                                 std::map<int, Number> values) {
    auto rootType = type;
    while (auto arrayType = dynamic_cast<ir::ArrayType *>(rootType))
      rootType = arrayType->baseType();

    for (auto i : values) {
      if (rootType == ir::BasicType::I32)
        i.second = IntNumber(i.second.intValue());
      else if (rootType == ir::BasicType::FLOAT)
        i.second = FloatNumber(i.second.floatValue());
      else
        throw std::runtime_error("Unsupported type");
    }

    auto symbol =
        new ir::GlobalVariable(isConst, type, name, fuseConst(type, values, 0));
    _table.front()[name] = symbol;
    return symbol;
  }

  size_t size() const { return _table.size(); }

  ir::AllocaInst *makeLocal(ir::BasicBlock *block, ir::Type *type,
                            const std::string &name) {
    auto symbol = new ir::AllocaInst(block, type);
    _table.front()[name] = symbol;
    return symbol;
  }

  ir::AllocaInst *makeLocal(ir::BasicBlock *block, ir::Type *type,
                            const std::string &name,
                            const std::vector<int> &dimensions) {
    for (int i = dimensions.size() - 1; i >= 0; i--)
      type = new ir::ArrayType(type, dimensions[i]);

    auto allocaInst = new ir::AllocaInst(block, type);
    _table.front()[name] = allocaInst;
    return allocaInst;
  }

  ir::Argument *makeArg(ir::Type *type, const std::string &name) {
    auto arg = new ir::Argument(type, name);
    _table.front()[name] = arg;
    return arg;
  }

  void out() { _table.pop_front(); }
};

// WIP
class ASTVisitor : public SysYBaseVisitor {
private:
  SysYParser::RootContext *const _rootAST;
  ir::Module *const _module = new ir::Module();
  SymbolTable *const _symbolTable = new SymbolTable();
  std::unordered_map<ir::Argument *, ir::AllocaInst *> _argToAllocaMap;
  std::deque<ir::BasicBlock *> _continueStack;
  std::deque<ir::BasicBlock *> _breakStack;
  bool _isProcessed = false;
  bool _isConst;
  ir::Function *_curFunc;
  ir::Value *_curValue;
  ir::Type *_curType;
  ir::BasicBlock *_entryBlock, *_retBlock, *_curBlock, *_trueBlock,
      *_falseBlock;

  void initBuiltInFuncs() {
    _module->addFunction(_symbolTable->makeFunc(ir::BasicType::I32, "getint"));
    _module->addFunction(_symbolTable->makeFunc(ir::BasicType::I32, "getch"));
    _module->addFunction(
        _symbolTable->makeFunc(ir::BasicType::I32, "getarray")
            ->addArg(new ir::Argument(new ir::PointerType(ir::BasicType::I32),
                                      "a")));
    _module->addFunction(
        _symbolTable->makeFunc(ir::BasicType::FLOAT, "getfloat"));
    _module->addFunction(
        _symbolTable->makeFunc(ir::BasicType::I32, "getfarray")
            ->addArg(new ir::Argument(new ir::PointerType(ir::BasicType::FLOAT),
                                      "a")));
    _module->addFunction(
        _symbolTable->makeFunc(ir::BasicType::VOID, "putint")
            ->addArg(new ir::Argument(ir::BasicType::I32, "a")));
    _module->addFunction(
        _symbolTable->makeFunc(ir::BasicType::VOID, "putch")
            ->addArg(new ir::Argument(ir::BasicType::I32, "a")));
    _module->addFunction(
        _symbolTable->makeFunc(ir::BasicType::VOID, "putarray")
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

  void initSysCalls() {
    _module->addFunction(
        _symbolTable->makeFunc(ir::BasicType::VOID, "memset")
            ->addArg(new ir::Argument(new ir::PointerType(ir::BasicType::I32),
                                      "addr"))
            ->addArg(new ir::Argument(ir::BasicType::I32, "value"))
            ->addArg(new ir::Argument(ir::BasicType::I32, "size")));
  }

  void checkIfIsProcessed() {
    if (_isProcessed)
      return;
    _isProcessed = true;
    visitRoot(_rootAST);
    formatLLVM();
  }

  void formatLLVM() {
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

  void allocInitVal(std::vector<int> &dimensions,
                    std::map<int, SysYParser::AdditiveExpContext *> &exps,
                    int base, SysYParser::InitValContext *src) {
    int offset = 0;
    for (auto exp : src->initVal()) {
      if (exp->additiveExp() == nullptr) {
        int size = 1;
        for (auto i = 1; i < dimensions.size(); i++)
          size *= dimensions[i];
        offset = (offset + size - 1) / size * size;
        std::vector<int> newDimensions(dimensions.begin() + 1,
                                       dimensions.end());
        allocInitVal(newDimensions, exps, base + offset, exp);
        offset += size;
      } else {
        exps[base + offset] = exp->additiveExp();
        offset++;
      }
    }
  }

public:
  ASTVisitor(SysYParser::RootContext *rootAST) : _rootAST(rootAST) {
    _symbolTable->in();
    initBuiltInFuncs();
    initSysCalls();
  }

  ir::Module *getModule() {
    checkIfIsProcessed();
    return _module;
  }

  std::any visitType(SysYParser::TypeContext *ctx) override {
    if (ctx->getText() == "int")
      return ir::BasicType::I32;
    else if (ctx->getText() == "float")
      return ir::BasicType::FLOAT;
    else if (ctx->getText() == "void")
      return ir::BasicType::VOID;
    else
      throw std::runtime_error("Unsupported type: " + ctx->getText());
  }

  std::any visitDimensions(SysYParser ::DimensionsContext *ctx) override {
    auto dimensions = std::vector<int>();
    for (auto exp : ctx->additiveExp()) {
      dimensions.push_back(
          std::any_cast<ir::ConstantNumber *>(visitAdditiveExp(exp))
              ->intValue());
    }

    return dimensions;
  }

  std::any visitVarDecl(SysYParser::VarDeclContext *ctx) override {
    _isConst = ctx->CONST() != nullptr;
    return SysYBaseVisitor::visitVarDecl(ctx);
  }

  std::any visitArrayVarDef(SysYParser::ArrayVarDefContext *ctx) override {
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
};

#endif