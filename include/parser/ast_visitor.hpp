#ifndef PARSER_AST_VISITOR_HPP
#define PARSER_AST_VISITOR_HPP

#include "SysYBaseVisitor.h"
#include "ir/alloca_inst.hpp"
#include "ir/argument.hpp"
#include "ir/array_type.hpp"
#include "ir/basic_block.hpp"
#include "ir/basic_type.hpp"
#include "ir/constant.hpp"
#include "ir/constant_array.hpp"
#include "ir/constant_number.hpp"
#include "ir/constant_zero.hpp"
#include "ir/function.hpp"
#include "ir/module.hpp"
#include "ir/pointer_type.hpp"
#include "ir/type.hpp"
#include "ir/value.hpp"
#include "model/number.hpp"
#include <list>
#include <map>
#include <stdexcept>
#include <unordered_map>

// WIP
class SymbolTable {
private:
  std::list<std::unordered_map<std::string, ir::Value *>> _table;

  ir::Value *get(const std::string &name) {
    for (auto item : _table) {
      if (item.contains(name))
        return item[name];
    }
    throw std::runtime_error("Undefined variable: " + name);
  }

  template <typename T>
  std::map<int, T> submap(std::map<int, T> &values, T fromKey, T toKey) {
    std::map<int, T> result;
    for (auto i : values)
      if (i.first >= fromKey && i.first <= toKey)
        result[i.first] = i.second;
    return result;
  }

  ir::Constant *fuseConst(ir::Type *type, std::map<int, Number> &values,
                          int base) {
    if (dynamic_cast<ir::BasicType *>(type))
      if (values.contains(base))
        return new ir::ConstantNumber(values[base]);
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

  template <typename T>
  ir::GlobalVariable *makeGlobal(bool isConst, ir::Type *type,
                                 const std::string &name, T value) {
    auto symbol = new ir::GlobalVariable(isConst, type, name,
                                         new ir::ConstantNumber<T>(value));
    _table.front()[name] = symbol;
    return symbol;
  }

  template <typename T>
  ir::GlobalVariable *makeGlobal(bool isConst, ir::Type *type,
                                 const std::string &name,
                                 std::map<int, T> values) {
    auto rootType = type;
    while (auto arrayType = dynamic_cast<ir::ArrayType *>(rootType))
      rootType = arrayType->baseType();

    for (auto i : values) {
    }
  }
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