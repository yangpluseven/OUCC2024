#ifndef PARSER_AST_VISITOR_HPP
#define PARSER_AST_VISITOR_HPP

#include "SysYBaseVisitor.h"
#include "ir/instruction.hpp"
#include "ir/basic_block.hpp"
#include "ir/constant.hpp"
#include "ir/function.hpp"
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
  // TODO:Declar SymbolTable
  std::unordered_map<ir::Argument *, ir::AllocaInst *> _argToAllocaMap;
  std::deque<ir::BasicBlock *> _continueStack;
  std::deque<ir::BasicBlock *> _breakStack;
  bool isProcessed = false;
  bool isConst;
  ir::Function *curFunc;
  ir::Value *curValue;
  ir::Type *curType;
  ir::BasicBlock *entryBlock, *retBlock, *curBlock, *trueBlock, *falseBlock;

public:
  ASTVisitor(SysYParser::RootContext *rootAST) : _rootAST(rootAST) {}
};

#endif