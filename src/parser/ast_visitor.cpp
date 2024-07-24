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

void ASTVisitor::processValueCond(ir::Value *value) {
  if (value != nullptr) {
    ir::Value *cond = nullptr;
    if (value->getType() == ir::BasicType::I1) {
      cond = value;
    } else if (value->getType() == ir::BasicType::I32) {
      ir::Instruction *inst =
          new ir::ICmpInst(_curBlock, ir::ICmpInst::NE, value,
                           new ir::ConstantNumber(model::Number(0)));
      _curBlock->add(inst);
      cond = inst;
    } else if (value->getType() == ir::BasicType::FLOAT) {
      ir::Instruction *inst =
          new ir::FCmpInst(_curBlock, ir::FCmpInst::UNE, value,
                           new ir::ConstantNumber(model::Number(0.0f)));
      _curBlock->add(inst);
      cond = inst;
    } else {
      throw std::runtime_error("Unsupported type: " +
                               value->getType()->toString());
    }
    _curBlock->add(new ir::BranchInst(_curBlock, cond, this->_trueBlock,
                                      this->_falseBlock));
  }
}

ir::Type *ASTVisitor::automaticTypePromotion(ir::Type *type1, ir::Type *type2) {
  if (type1 == ir::BasicType::I32 || type2 == ir::BasicType::I32)
    return ir::BasicType::I32;
  if (type1 == ir::BasicType::FLOAT || type2 == ir::BasicType::FLOAT)
    return ir::BasicType::FLOAT;
  return ir::BasicType::I1;
}

ir::Value *ASTVisitor::typeConversion(ir::Value *value, ir::Type *targetType) {
  if (value->getType() == targetType)
    return value;
  if (auto constant = dynamic_cast<ir::ConstantNumber *>(value)) {
    if (targetType == ir::BasicType::I32)
      return new ir::ConstantNumber(constant->intValue());
    else if (targetType == ir::BasicType::I1)
      return new ir::ConstantNumber(constant->intValue() != 0);
    else if (targetType == ir::BasicType::FLOAT)
      return new ir::ConstantNumber(constant->floatValue());
    else
      throw std::runtime_error("Unsupported type: " + targetType->toString());
  }
  if (targetType == ir::BasicType::I1) {
    if (value->getType() == ir::BasicType::I32) {
      ir::Instruction *inst =
          new ir::ICmpInst(_curBlock, ir::ICmpInst::NE, value,
                           new ir::ConstantNumber(model::Number(0)));
      _curBlock->add(inst);
      return inst;
    } else if (value->getType() == ir::BasicType::FLOAT) {
      ir::Instruction *inst =
          new ir::FCmpInst(_curBlock, ir::FCmpInst::UNE, value,
                           new ir::ConstantNumber(model::Number(0.0f)));
      _curBlock->add(inst);
      return inst;
    } else
      return value;
  }
  if (targetType == ir::BasicType::I32) {
    if (value->getType() == ir::BasicType::I1) {
      ir::Instruction *inst =
          new ir::ZExtInst(_curBlock, ir::BasicType::I32, value);
      _curBlock->add(inst);
      return inst;
    } else if (value->getType() == ir::BasicType::FLOAT) {
      ir::Instruction *inst =
          new ir::FPToSIInst(_curBlock, ir::BasicType::I32, value);
      _curBlock->add(inst);
      return inst;
    } else
      return value;
  }
  if (targetType == ir::BasicType::FLOAT) {
    if (value->getType() == ir::BasicType::I1) {
      ir::Instruction *inst =
          new ir::ZExtInst(_curBlock, ir::BasicType::I32, value);
      _curBlock->add(inst);
      inst = new ir::SIToFPInst(_curBlock, ir::BasicType::FLOAT, inst);
      _curBlock->add(inst);
      return inst;
    } else if (value->getType() == ir::BasicType::I32) {
      ir::Instruction *inst =
          new ir::SIToFPInst(_curBlock, ir::BasicType::FLOAT, value);
      _curBlock->add(inst);
      return inst;
    } else
      return value;
  }
  return value;
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
    std::reverse(reverseDimensions.begin(), reverseDimensions.end());
    auto type = _curType;
    for (auto i : reverseDimensions) {
      type = new ir::ArrayType(type, i);
    }

    std::map<int, model::Number> values;
    for (const auto &entry : exps) {
      int index = entry.first;
      auto exp = entry.second;
      auto result = std::any_cast<ir::ConstantNumber *>(visitAdditiveExp(exp));
      values.insert(std::make_pair(index, result->getValue()));
    }

    _module->addGlobal(_symbolTable->makeGlobal(_isConst, type, name, values));
    return nullptr;
  }
  auto allocaInst =
      _symbolTable->makeLocal(_entryBlock, _curType, name, dimensions);
  _entryBlock->add(allocaInst);
  if (initVal != nullptr) {
    std::map<int, SysYParser::AdditiveExpContext *> exps;
    allocInitVal(dimensions, exps, 0, initVal);
    auto bitCastInst = new ir::BitCastInst(
        _curBlock, new ir::PointerType(ir::BasicType::I32), allocaInst);
    _curBlock->add(bitCastInst);
    int x = 4;
    for (auto i : dimensions)
      x *= i;

    _curBlock->add(
        new ir::CallInst(_curBlock, _symbolTable->getFunc("memset"),
                         {bitCastInst, new ir::ConstantNumber(model::Number(0)),
                          new ir::ConstantNumber(model::Number(x))}));

    for (const auto &entry : exps) {
      auto value = typeConversion(
          std::any_cast<ir::Value *>(visitAdditiveExp(entry.second)), _curType);
      ir::Value *ptr = allocaInst;
      for (int j = 0; j < dimensions.size(); j++) {
        int x = 1;
        for (auto k = j + 1; k < dimensions.size(); k++)
          x *= dimensions[k];
        int index = entry.first / x % dimensions[j];
        auto inst = new ir::GetElementPtrInst(
            _curBlock, ptr,
            {new ir::ConstantNumber(model::Number(0)),
             new ir::ConstantNumber(model::Number(index))});
        _curBlock->add(inst);
        ptr = inst;
      }
      _curBlock->add(new ir::StoreInst(_curBlock, value, ptr));
    }
  }
  return nullptr;
}

std::any ASTVisitor::visitFuncDef(SysYParser::FuncDefContext *ctx) {
  _argToAllocaMap.clear();
  auto funcType = std::any_cast<ir::Type *>(visitType(ctx->type()));
  _curFunc =
      _symbolTable->makeFunc(funcType, ctx->Ident()->getSymbol()->getText());
  _symbolTable->in();
  _entryBlock = new ir::BasicBlock(_curFunc);
  _curFunc->add(_entryBlock);
  _retBlock = new ir::BasicBlock(_curFunc);
  if (_curFunc->getType() == ir::BasicType::I32 ||
      _curFunc->getType() == ir::BasicType::FLOAT) {
    auto allocaInst = new ir::AllocaInst(_entryBlock, _curFunc->getType());
    _entryBlock->add(allocaInst);
    _curRetVal = allocaInst;
    auto loadInst = new ir::LoadInst(_retBlock, _curRetVal);
    _retBlock->add(loadInst);
    _retBlock->add(new ir::RetInst(_retBlock, loadInst));
  } else if (_curFunc->getType() == ir::BasicType::VOID) {
    _curRetVal = nullptr;
    _retBlock->add(new ir::RetInst(_retBlock));
  } else
    throw std::runtime_error("Unsupported type: " +
                             _curFunc->getType()->toString());
  _curBlock = _entryBlock;
  _curFunc->add(_retBlock);
  for (auto argCtx : ctx->funcArg()) {
    auto arg = std::any_cast<ir::Argument *>(visitFuncArg(argCtx));
    _curFunc->addArg(arg);
    auto allocaInst =
        _symbolTable->makeLocal(_entryBlock, arg->getType(), arg->getName());
    _entryBlock->add(allocaInst);
    _curBlock->add(new ir::StoreInst(_curBlock, arg, allocaInst));
    _argToAllocaMap.insert(std::make_pair(arg, allocaInst));
  }
  visitBlockStmt(ctx->blockStmt());
  if (_curFunc->getType() != ir::BasicType::VOID) {
    ir::Constant *retVal;
    if (_curFunc->getType() == ir::BasicType::I32)
      retVal = new ir::ConstantNumber(model::Number(0));
    else if (_curFunc->getType() == ir::BasicType::FLOAT)
      retVal = new ir::ConstantNumber(model::Number(0.0f));
    else
      throw std::runtime_error("Unsupported type: " +
                               _curFunc->getType()->toString());

    _entryBlock->add(new ir::StoreInst(_entryBlock, retVal, _curRetVal));
  }
  _entryBlock->add(new ir::BranchInst(_entryBlock, _curFunc->get(1)));
  _curFunc->add(_retBlock);
  _module->addFunction(_curFunc);
  _symbolTable->out();
  return nullptr;
}

std::any ASTVisitor::visitFuncArg(SysYParser::FuncArgContext *ctx) {
  visitType(ctx->type());
  auto type = std::any_cast<ir::Type *>(visitType(ctx->type()));
  if (!ctx->LB().empty()) {
    auto reversed_ctx = ctx->additiveExp();
    std::reverse(reversed_ctx.begin(), reversed_ctx.end());
    for (auto exp : reversed_ctx)
      type = new ir::ArrayType(
          type, std::any_cast<ir::ConstantNumber *>(visitAdditiveExp(exp))
                    ->intValue());
    type = new ir::PointerType(type);
  }
  return _symbolTable->makeArg(type, ctx->Ident()->getSymbol()->getText());
}

std::any ASTVisitor::visitBlockStmt(SysYParser::BlockStmtContext *ctx) {
  _symbolTable->in();
  for (auto stmt : ctx->stmt()) {
    visitStmt(stmt);
    if (stmt->continueStmt() != nullptr || stmt->breakStmt() != nullptr ||
        stmt->retStmt() != nullptr)
      break;
  }
  _symbolTable->out();
  return nullptr;
}

std::any ASTVisitor::visitAssignStmt(SysYParser::AssignStmtContext *ctx) {
  auto ptr = std::any_cast<ir::Value *>(visitLVal(ctx->lVal()));
  auto value = std::any_cast<ir::Value *>(visitAdditiveExp(ctx->additiveExp()));
  auto type = ptr->getType();
  if (dynamic_cast<ir::BasicType *>(type) != nullptr)
    value = typeConversion(value, type);
  else
    value = typeConversion(value, type->baseType());
  _curBlock->add(new ir::StoreInst(_curBlock, value, ptr));
  return nullptr;
}

std::any ASTVisitor::visitScalarVarDef(SysYParser::ScalarVarDefContext *ctx) {
  auto name = ctx->Ident()->getSymbol()->getText();
  if (_isConst || _symbolTable->size() == 1) {
    auto value = model::Number(0);
    if (ctx->additiveExp() != nullptr)
      value = std::any_cast<ir::ConstantNumber *>(
                  visitAdditiveExp(ctx->additiveExp()))
                  ->getValue();
    _module->addGlobal(
        _symbolTable->makeGlobal(_isConst, _curType, name, value));
    return nullptr;
  }
  auto allocaInst = _symbolTable->makeLocal(_entryBlock, _curType, name);
  _entryBlock->add(allocaInst);
  auto valueExp = ctx->additiveExp();
  if (valueExp != nullptr) {
    auto value = std::any_cast<ir::Value *>(visitAdditiveExp(valueExp));
    value = typeConversion(value, _curType);
    _curBlock->add(new ir::StoreInst(_curBlock, value, allocaInst));
  }
  return nullptr;
}

std::any ASTVisitor::visitIfElseStmt(SysYParser::IfElseStmtContext *ctx) {
  auto trueBlock = new ir::BasicBlock(_curFunc);
  auto falseBlock = new ir::BasicBlock(_curFunc);
  auto ifEndBlock = new ir::BasicBlock(_curFunc);
  _curFunc->insertAfter(_curBlock, trueBlock);
  _curFunc->insertAfter(trueBlock, falseBlock);
  _curFunc->insertAfter(falseBlock, ifEndBlock);
  _trueBlock = trueBlock;
  _falseBlock = falseBlock;
  auto value = std::any_cast<ir::Value *>(visitLorExp(ctx->lorExp()));
  processValueCond(value);
  _curBlock = trueBlock;
  visitStmt(ctx->stmt(0));
  if (_curBlock->isEmpty() ||
      !(dynamic_cast<ir::BranchInst *>(_curBlock->getLast()) != nullptr ||
        dynamic_cast<ir::RetInst *>(_curBlock->getLast()) != nullptr))
    _curBlock->add(new ir::BranchInst(_curBlock, ifEndBlock));
  _curBlock = falseBlock;
  visitStmt(ctx->stmt(1));
  if (_curBlock->isEmpty() ||
      !(dynamic_cast<ir::BranchInst *>(_curBlock->getLast()) != nullptr ||
        dynamic_cast<ir::RetInst *>(_curBlock->getLast()) != nullptr))
    _curBlock->add(new ir::BranchInst(_curBlock, ifEndBlock));
  _curBlock = ifEndBlock;
  return nullptr;
}

std::any ASTVisitor::visitIfStmt(SysYParser::IfStmtContext *ctx) {
  auto trueBlock = new ir::BasicBlock(_curFunc);
  auto falseBlock = new ir::BasicBlock(_curFunc);
  _curFunc->insertAfter(_curBlock, trueBlock);
  _curFunc->insertAfter(trueBlock, falseBlock);
  _trueBlock = trueBlock;
  _falseBlock = falseBlock;
  auto value = std::any_cast<ir::Value *>(visitLorExp(ctx->lorExp()));
  processValueCond(value);
  _curBlock = trueBlock;
  visitStmt(ctx->stmt());
  if (_curBlock->isEmpty() ||
      !(dynamic_cast<ir::BranchInst *>(_curBlock->getLast()) != nullptr ||
        dynamic_cast<ir::RetInst *>(_curBlock->getLast()) != nullptr))
    _curBlock->add(new ir::BranchInst(_curBlock, falseBlock));
  _curBlock = falseBlock;
  return nullptr;
}

std::any ASTVisitor::visitWhileStmt(SysYParser::WhileStmtContext *ctx) {
  auto entryBlock = new ir::BasicBlock(_curFunc);
  auto loopBlock = new ir::BasicBlock(_curFunc);
  auto endBlock = new ir::BasicBlock(_curFunc);
  _curFunc->insertAfter(_curBlock, entryBlock);
  _curFunc->insertAfter(entryBlock, loopBlock);
  _curFunc->insertAfter(loopBlock, endBlock);
  _continueStack.push_front(entryBlock);
  _breakStack.push_front(endBlock);
  if (_curBlock->isEmpty() ||
      !(dynamic_cast<ir::BranchInst *>(_curBlock->getLast()) != nullptr ||
        dynamic_cast<ir::RetInst *>(_curBlock->getLast()) != nullptr))
    _curBlock->add(new ir::BranchInst(_curBlock, entryBlock));
  _curBlock = entryBlock;
  _trueBlock = loopBlock;
  _falseBlock = endBlock;
  auto value = std::any_cast<ir::Value *>(visitLorExp(ctx->lorExp()));
  processValueCond(value);
  _curBlock = loopBlock;
  visitStmt(ctx->stmt());
  if (_curBlock->isEmpty() ||
      !(dynamic_cast<ir::BranchInst *>(_curBlock->getLast()) != nullptr ||
        dynamic_cast<ir::RetInst *>(_curBlock->getLast()) != nullptr))
    _curBlock->add(new ir::BranchInst(_curBlock, entryBlock));
  _curBlock = endBlock;
  _continueStack.pop_front();
  _breakStack.pop_front();
  return nullptr;
}

std::any ASTVisitor::visitBreakStmt(SysYParser::BreakStmtContext *ctx) {
  _curBlock->add(new ir::BranchInst(_curBlock, _breakStack.front()));
  return nullptr;
}

std::any ASTVisitor::visitContinueStmt(SysYParser::ContinueStmtContext *ctx) {
  _curBlock->add(new ir::BranchInst(_curBlock, _continueStack.front()));
  return nullptr;
}

std::any ASTVisitor::visitRetStmt(SysYParser::RetStmtContext *ctx) {
  if(ctx->additiveExp()==nullptr){
    _curBlock->add(new ir::BranchInst(_curBlock, _retBlock));
    return nullptr;
  }
  auto retVal = std::any_cast<ir::Value *>(visitAdditiveExp(ctx->additiveExp()));
  retVal = typeConversion(retVal, _curFunc->getType());
  _curBlock->add(new ir::StoreInst(_curBlock, retVal, _curRetVal));
  _curBlock->add(new ir::BranchInst(_curBlock, _retBlock));
  return nullptr;
}
} // namespace parser