#include "ast_visitor.hpp"

namespace parser {

void ASTVisitor::initBuiltInFuncs() {
  _module->addFunction(_symbolTable->makeFunc(ir::BasicType::I32, "getint"));
  _module->addFunction(_symbolTable->makeFunc(ir::BasicType::I32, "getch"));
  _module->addFunction(_symbolTable->makeFunc(ir::BasicType::I32, "getarray")
                           ->pushArg(new ir::Argument(
                               new ir::PointerType(ir::BasicType::I32), "a")));
  _module->addFunction(
      _symbolTable->makeFunc(ir::BasicType::FLOAT, "getfloat"));
  _module->addFunction(
      _symbolTable->makeFunc(ir::BasicType::I32, "getfarray")
          ->pushArg(new ir::Argument(new ir::PointerType(ir::BasicType::FLOAT),
                                    "a")));
  _module->addFunction(_symbolTable->makeFunc(ir::BasicType::VOID, "putint")
                           ->pushArg(new ir::Argument(ir::BasicType::I32, "a")));
  _module->addFunction(_symbolTable->makeFunc(ir::BasicType::VOID, "putch")
                           ->pushArg(new ir::Argument(ir::BasicType::I32, "a")));
  _module->addFunction(_symbolTable->makeFunc(ir::BasicType::VOID, "putarray")
                           ->pushArg(new ir::Argument(ir::BasicType::I32, "n"))
                           ->pushArg(new ir::Argument(ir::BasicType::I32, "a")));
  _module->addFunction(
      _symbolTable->makeFunc(ir::BasicType::VOID, "putfloat")
          ->pushArg(new ir::Argument(ir::BasicType::FLOAT, "a")));
  _module->addFunction(
      _symbolTable->makeFunc(ir::BasicType::VOID, "putfarray")
          ->pushArg(new ir::Argument(ir::BasicType::I32, "n"))
          ->pushArg(new ir::Argument(ir::BasicType::FLOAT, "a")));
  _module->addFunction(
      _symbolTable->makeFunc(ir::BasicType::VOID, "_sysy_starttime")
          ->pushArg(new ir::Argument(ir::BasicType::I32, "lineno")));
  _module->addFunction(
      _symbolTable->makeFunc(ir::BasicType::VOID, "_sysy_stoptime")
          ->pushArg(new ir::Argument(ir::BasicType::I32, "lineno")));
}

void ASTVisitor::initSysCalls() {
  _module->addFunction(
      _symbolTable->makeFunc(ir::BasicType::VOID, "memset")
          ->pushArg(
              new ir::Argument(new ir::PointerType(ir::BasicType::I32), "addr"))
          ->pushArg(new ir::Argument(ir::BasicType::I32, "value"))
          ->pushArg(new ir::Argument(ir::BasicType::I32, "size")));
}

void ASTVisitor::checkIfIsProcessed() {
  if (_isProcessed)
    return;
  _isProcessed = true;
  visitRoot(_rootAST);
  formatLLVM();
}

void ASTVisitor::formatLLVM() {
  for (const auto func : _module->getFunctions()) {
    if (func->isDeclare())
      continue;
    for (int i = 0; i < func->size(); i++) {
      const auto block = func->get(i);
      for (int j = 0; j + 1 < block->size(); j++) {
        auto terminal = block->get(j);
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
        block->insert(new ir::BranchInst(block, func->get(i + 1)));
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
    ir::Value *cond;
    if (value->getType() == ir::BasicType::I1) {
      cond = value;
    } else if (value->getType() == ir::BasicType::I32) {
      ir::Instruction *inst =
          new ir::ICmpInst(_curBlock, ir::ICmpInst::NE, value,
                           new ir::ConstantNumber(model::Number(0)));
      _curBlock->insert(inst);
      cond = inst;
    } else if (value->getType() == ir::BasicType::FLOAT) {
      ir::Instruction *inst =
          new ir::FCmpInst(_curBlock, ir::FCmpInst::UNE, value,
                           new ir::ConstantNumber(model::Number(0.0f)));
      _curBlock->insert(inst);
      cond = inst;
    } else {
      throw std::runtime_error("Unsupported type: " +
                               value->getType()->str());
    }
    _curBlock->insert(new ir::BranchInst(_curBlock, cond, this->_trueBlock,
                                      this->_falseBlock));
  }
}

ir::Type *ASTVisitor::automaticTypePromotion(ir::Type *type1, ir::Type *type2) {
  if (type1 == ir::BasicType::FLOAT || type2 == ir::BasicType::FLOAT)
    return ir::BasicType::FLOAT;
  if (type1 == ir::BasicType::I32 || type2 == ir::BasicType::I32)
    return ir::BasicType::I32;
  return ir::BasicType::I1;
}

ir::Value *ASTVisitor::typeConversion(ir::Value *value,
                                      ir::Type *targetType) const {
  if (value->getType() == targetType)
    return value;
  if (auto constant = dynamic_cast<ir::ConstantNumber *>(value)) {
    if (targetType == ir::BasicType::I32)
      return new ir::ConstantNumber(model::Number(constant->intValue()));
    else if (targetType == ir::BasicType::I1)
      return new ir::ConstantNumber(constant->intValue() != 0);
    else if (targetType == ir::BasicType::FLOAT)
      return new ir::ConstantNumber(model::Number(constant->floatValue()));
    else
      throw std::runtime_error("Unsupported type: " + targetType->str());
  }
  if (targetType == ir::BasicType::I1) {
    if (value->getType() == ir::BasicType::I32) {
      ir::Instruction *inst =
          new ir::ICmpInst(_curBlock, ir::ICmpInst::NE, value,
                           new ir::ConstantNumber(model::Number(0)));
      _curBlock->insert(inst);
      return inst;
    } else if (value->getType() == ir::BasicType::FLOAT) {
      ir::Instruction *inst =
          new ir::FCmpInst(_curBlock, ir::FCmpInst::UNE, value,
                           new ir::ConstantNumber(model::Number(0.0f)));
      _curBlock->insert(inst);
      return inst;
    } else
      return value;
  }
  if (targetType == ir::BasicType::I32) {
    if (value->getType() == ir::BasicType::I1) {
      ir::Instruction *inst =
          new ir::ZExtInst(_curBlock, ir::BasicType::I32, value);
      _curBlock->insert(inst);
      return inst;
    } else if (value->getType() == ir::BasicType::FLOAT) {
      ir::Instruction *inst =
          new ir::FPToSIInst(_curBlock, ir::BasicType::I32, value);
      _curBlock->insert(inst);
      return inst;
    } else
      return value;
  }
  if (targetType == ir::BasicType::FLOAT) {
    if (value->getType() == ir::BasicType::I1) {
      ir::Instruction *inst =
          new ir::ZExtInst(_curBlock, ir::BasicType::I32, value);
      _curBlock->insert(inst);
      inst = new ir::SIToFPInst(_curBlock, ir::BasicType::FLOAT, inst);
      _curBlock->insert(inst);
      return inst;
    } else if (value->getType() == ir::BasicType::I32) {
      ir::Instruction *inst =
          new ir::SIToFPInst(_curBlock, ir::BasicType::FLOAT, value);
      _curBlock->insert(inst);
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
  auto txt = ctx->getText();
  if (txt == "int")
    _curType = ir::BasicType::I32;
  else if (txt == "float")
    _curType = ir::BasicType::FLOAT;
  else if (txt == "void")
    _curType = ir::BasicType::VOID;
  else
    throw std::runtime_error("Unsupported type: " + txt);
  return std::make_any<ir::Type *>(_curType);
}

std::any ASTVisitor::visitDimensions(SysYParser::DimensionsContext *ctx) {
  auto dimensions = std::vector<int>();
  for (auto exp : ctx->additiveExp()) {
    dimensions.push_back(dynamic_cast<ir::ConstantNumber *>(
                             std::any_cast<ir::Value *>(visitAdditiveExp(exp)))
                             ->intValue());
  }
  return std::make_any<std::vector<int>>(dimensions);
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
      auto result = dynamic_cast<ir::ConstantNumber *>(
          std::any_cast<ir::Value *>(visitAdditiveExp(exp)));
      values.insert(std::make_pair(index, result->getValue()));
    }

    _module->addGlobal(_symbolTable->makeGlobal(_isConst, type, name, values));
    return std::make_any<ir::Value *>(nullptr);
  }
  auto allocaInst =
      _symbolTable->makeLocal(_entryBlock, _curType, name, dimensions);
  _entryBlock->insert(allocaInst);
  if (initVal != nullptr) {
    std::map<int, SysYParser::AdditiveExpContext *> exps;
    allocInitVal(dimensions, exps, 0, initVal);
    auto bitCastInst = new ir::BitCastInst(
        _curBlock, new ir::PointerType(ir::BasicType::I32), allocaInst);
    _curBlock->insert(bitCastInst);
    int x = 4;
    for (auto i : dimensions)
      x *= i;

    _curBlock->insert(
        new ir::CallInst(_curBlock, _symbolTable->getFunc("memset"),
                         {bitCastInst, new ir::ConstantNumber(model::Number(0)),
                          new ir::ConstantNumber(model::Number(x))}));

    for (const auto &entry : exps) {
      auto value = typeConversion(
          std::any_cast<ir::Value *>(visitAdditiveExp(entry.second)), _curType);
      ir::Value *ptr = allocaInst;
      for (int j = 0; j < dimensions.size(); j++) {
        int num = 1;
        for (auto k = j + 1; k < dimensions.size(); k++)
          num *= dimensions[k];
        int index = entry.first / num % dimensions[j];
        auto inst = new ir::GetPtrInst(
            _curBlock, ptr,
            {new ir::ConstantNumber(model::Number(0)),
             new ir::ConstantNumber(model::Number(index))});
        _curBlock->insert(inst);
        ptr = inst;
      }
      _curBlock->insert(new ir::StoreInst(_curBlock, value, ptr));
    }
  }
  return std::make_any<ir::Value *>(nullptr);
}

std::any ASTVisitor::visitFuncDef(SysYParser::FuncDefContext *ctx) {
  _argToAllocaMap.clear();
  auto funcType = std::any_cast<ir::Type *>(visitType(ctx->type()));
  _curFunc =
      _symbolTable->makeFunc(funcType, ctx->Ident()->getSymbol()->getText());
  _symbolTable->in();
  _entryBlock = new ir::BasicBlock(_curFunc);
  _curFunc->pushBlock(_entryBlock);
  _retBlock = new ir::BasicBlock(_curFunc);
  if (_curFunc->getType() == ir::BasicType::I32 ||
      _curFunc->getType() == ir::BasicType::FLOAT) {
    auto allocaInst = new ir::AllocaInst(_entryBlock, _curFunc->getType());
    _entryBlock->insert(allocaInst);
    _curRetVal = allocaInst;
    auto loadInst = new ir::LoadInst(_retBlock, _curRetVal);
    _retBlock->insert(loadInst);
    _retBlock->insert(new ir::RetInst(_retBlock, loadInst));
  } else if (_curFunc->getType() == ir::BasicType::VOID) {
    _curRetVal = nullptr;
    _retBlock->insert(new ir::RetInst(_retBlock));
  } else
    throw std::runtime_error("Unsupported type: " +
                             _curFunc->getType()->str());
  _curBlock = new ir::BasicBlock(_curFunc);
  _curFunc->pushBlock(_curBlock);
  for (auto argCtx : ctx->funcArg()) {
    auto arg = dynamic_cast<ir::Argument *>(
        std::any_cast<ir::Value *>(visitFuncArg(argCtx)));
    _curFunc->pushArg(arg);
    auto allocaInst =
        _symbolTable->makeLocal(_entryBlock, arg->getType(), arg->getName());
    _entryBlock->insert(allocaInst);
    _curBlock->insert(new ir::StoreInst(_curBlock, arg, allocaInst));
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
                               _curFunc->getType()->str());

    _entryBlock->insert(new ir::StoreInst(_entryBlock, retVal, _curRetVal));
  }
  _entryBlock->insert(new ir::BranchInst(_entryBlock, _curFunc->get(1)));
  _curFunc->pushBlock(_retBlock);
  _module->addFunction(_curFunc);
  _symbolTable->out();
  return std::make_any<ir::Value *>(nullptr);
}

std::any ASTVisitor::visitFuncArg(SysYParser::FuncArgContext *ctx) {
  visitType(ctx->type());
  auto type = std::any_cast<ir::Type *>(visitType(ctx->type()));
  if (!ctx->LB().empty()) {
    auto reversed_ctx = ctx->additiveExp();
    std::reverse(reversed_ctx.begin(), reversed_ctx.end());
    for (auto exp : reversed_ctx)
      type = new ir::ArrayType(
          type, dynamic_cast<ir::ConstantNumber *>(
                    std::any_cast<ir::Value *>(visitAdditiveExp(exp)))
                    ->intValue());
    type = new ir::PointerType(type);
  }
  return std::make_any<ir::Value *>(
      _symbolTable->makeArg(type, ctx->Ident()->getSymbol()->getText()));
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
  return std::make_any<ir::Value *>(nullptr);
}

std::any ASTVisitor::visitAssignStmt(SysYParser::AssignStmtContext *ctx) {
  const auto ptr = std::any_cast<ir::Value *>(visitLVal(ctx->lVal()));
  auto value = std::any_cast<ir::Value *>(visitAdditiveExp(ctx->additiveExp()));
  const auto type = ptr->getType();
  if (dynamic_cast<ir::BasicType *>(type))
    value = typeConversion(value, type);
  else
    value = typeConversion(value, type->baseType());
  _curBlock->insert(new ir::StoreInst(_curBlock, value, ptr));
  return std::make_any<ir::Value *>(nullptr);
}

std::any ASTVisitor::visitScalarVarDef(SysYParser::ScalarVarDefContext *ctx) {
  auto name = ctx->Ident()->getSymbol()->getText();
  if (_isConst || _symbolTable->size() == 1) {
    auto value = model::Number(0);
    if (ctx->additiveExp() != nullptr)
      value =
          dynamic_cast<ir::ConstantNumber *>(
              std::any_cast<ir::Value *>(visitAdditiveExp(ctx->additiveExp())))
              ->getValue();
    _module->addGlobal(
        _symbolTable->makeGlobal(_isConst, _curType, name, value));
    return std::make_any<ir::Value *>(nullptr);
  }
  auto allocaInst = _symbolTable->makeLocal(_entryBlock, _curType, name);
  _entryBlock->insert(allocaInst);
  auto valueExp = ctx->additiveExp();
  if (valueExp != nullptr) {
    auto value = std::any_cast<ir::Value *>(visitAdditiveExp(valueExp));
    value = typeConversion(value, _curType);
    _curBlock->insert(new ir::StoreInst(_curBlock, value, allocaInst));
  }
  return std::make_any<ir::Value *>(nullptr);
}

std::any ASTVisitor::visitIfElseStmt(SysYParser::IfElseStmtContext *ctx) {
  auto trueBlock = new ir::BasicBlock(_curFunc);
  auto falseBlock = new ir::BasicBlock(_curFunc);
  auto ifEndBlock = new ir::BasicBlock(_curFunc);
  _curFunc->insertBlock(_curBlock, trueBlock);
  _curFunc->insertBlock(trueBlock, falseBlock);
  _curFunc->insertBlock(falseBlock, ifEndBlock);
  _trueBlock = trueBlock;
  _falseBlock = falseBlock;
  auto value = std::any_cast<ir::Value *>(visitLorExp(ctx->lorExp()));
  processValueCond(value);
  _curBlock = trueBlock;
  visitStmt(ctx->stmt(0));
  if (_curBlock->isEmpty() ||
      !(dynamic_cast<ir::BranchInst *>(_curBlock->getLast()) != nullptr ||
        dynamic_cast<ir::RetInst *>(_curBlock->getLast()) != nullptr))
    _curBlock->insert(new ir::BranchInst(_curBlock, ifEndBlock));
  _curBlock = falseBlock;
  visitStmt(ctx->stmt(1));
  if (_curBlock->isEmpty() ||
      !(dynamic_cast<ir::BranchInst *>(_curBlock->getLast()) != nullptr ||
        dynamic_cast<ir::RetInst *>(_curBlock->getLast()) != nullptr))
    _curBlock->insert(new ir::BranchInst(_curBlock, ifEndBlock));
  _curBlock = ifEndBlock;
  return std::make_any<ir::Value *>(nullptr);
}

std::any ASTVisitor::visitIfStmt(SysYParser::IfStmtContext *ctx) {
  auto trueBlock = new ir::BasicBlock(_curFunc);
  auto falseBlock = new ir::BasicBlock(_curFunc);
  _curFunc->insertBlock(_curBlock, trueBlock);
  _curFunc->insertBlock(trueBlock, falseBlock);
  _trueBlock = trueBlock;
  _falseBlock = falseBlock;
  auto value = std::any_cast<ir::Value *>(visitLorExp(ctx->lorExp()));
  processValueCond(value);
  _curBlock = trueBlock;
  visitStmt(ctx->stmt());
  if (_curBlock->isEmpty() ||
      !(dynamic_cast<ir::BranchInst *>(_curBlock->getLast()) != nullptr ||
        dynamic_cast<ir::RetInst *>(_curBlock->getLast()) != nullptr))
    _curBlock->insert(new ir::BranchInst(_curBlock, falseBlock));
  _curBlock = falseBlock;
  return std::make_any<ir::Value *>(nullptr);
}

std::any ASTVisitor::visitWhileStmt(SysYParser::WhileStmtContext *ctx) {
  auto entryBlock = new ir::BasicBlock(_curFunc);
  auto loopBlock = new ir::BasicBlock(_curFunc);
  auto endBlock = new ir::BasicBlock(_curFunc);
  _curFunc->insertBlock(_curBlock, entryBlock);
  _curFunc->insertBlock(entryBlock, loopBlock);
  _curFunc->insertBlock(loopBlock, endBlock);
  _continueStack.push_front(entryBlock);
  _breakStack.push_front(endBlock);
  if (_curBlock->isEmpty() ||
      !(dynamic_cast<ir::BranchInst *>(_curBlock->getLast()) != nullptr ||
        dynamic_cast<ir::RetInst *>(_curBlock->getLast()) != nullptr))
    _curBlock->insert(new ir::BranchInst(_curBlock, entryBlock));
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
    _curBlock->insert(new ir::BranchInst(_curBlock, entryBlock));
  _curBlock = endBlock;
  _continueStack.pop_front();
  _breakStack.pop_front();
  return std::make_any<ir::Value *>(nullptr);
}

std::any ASTVisitor::visitBreakStmt(SysYParser::BreakStmtContext *ctx) {
  _curBlock->insert(new ir::BranchInst(_curBlock, _breakStack.front()));
  return std::make_any<ir::Value *>(nullptr);
}

std::any ASTVisitor::visitContinueStmt(SysYParser::ContinueStmtContext *ctx) {
  _curBlock->insert(new ir::BranchInst(_curBlock, _continueStack.front()));
  return std::make_any<ir::Value *>(nullptr);
}

std::any ASTVisitor::visitRetStmt(SysYParser::RetStmtContext *ctx) {
  if (ctx->additiveExp() == nullptr) {
    _curBlock->insert(new ir::BranchInst(_curBlock, _retBlock));
    return std::make_any<ir::Value *>(nullptr);
  }
  auto retVal =
      std::any_cast<ir::Value *>(visitAdditiveExp(ctx->additiveExp()));
  retVal = typeConversion(retVal, _curFunc->getType());
  _curBlock->insert(new ir::StoreInst(_curBlock, retVal, _curRetVal));
  _curBlock->insert(new ir::BranchInst(_curBlock, _retBlock));
  return std::make_any<ir::Value *>(nullptr);
}

std::any
ASTVisitor::visitMultiplicativeExp(SysYParser::MultiplicativeExpContext *ctx) {
  auto iterVal =
      std::any_cast<ir::Value *>(ASTVisitor::visitUnaryExp(ctx->unaryExp(0)));
  for (int i = 1; i < ctx->unaryExp().size(); i++) {
    auto nextVal =
        std::any_cast<ir::Value *>(ASTVisitor::visitUnaryExp(ctx->unaryExp(i)));
    ir::Type *targetType = ASTVisitor::automaticTypePromotion(
        iterVal->getType(), nextVal->getType());
    iterVal = typeConversion(iterVal, targetType);
    nextVal = typeConversion(nextVal, targetType);
    auto number1 = dynamic_cast<ir::ConstantNumber *>(iterVal);
    auto number2 = dynamic_cast<ir::ConstantNumber *>(nextVal);
    auto txt = ctx->children[i * 2 - 1]->getText();
    if (number1 && number2) {
      if (txt == "*") {
        iterVal = number1->mul(number2);
      } else if (txt == "/") {
        iterVal = number1->div(number2);
      } else if (txt == "%") {
        iterVal = number1->rem(number2);
      } else {
        throw std::runtime_error("Invalid operation: " + txt);
      }
      continue;
    }
    ir::BinaryInst::Op tmp;
    if (txt == "*") {
      if (targetType == ir::BasicType::I32) {
        tmp = ir::BinaryInst::MUL;
      } else if (targetType == ir::BasicType::FLOAT) {
        tmp = ir::BinaryInst::FMUL;
      } else {
        throw std::runtime_error("Invalid type" + targetType->str());
      }
    } else if (txt == "/") {
      if (targetType == ir::BasicType::I32) {
        tmp = ir::BinaryInst::SDIV;
      } else if (targetType == ir::BasicType::FLOAT) {
        tmp = ir::BinaryInst::FDIV;
      } else {
        throw std::runtime_error("Invalid type" + targetType->str());
      }
    } else if (txt == "%") {
      tmp = ir::BinaryInst::SREM;
    } else {
      throw std::runtime_error("Invalid operation: " +
                               ctx->children[1]->getText());
    }
    ir::Instruction *inst =
        new ir::BinaryInst(_curBlock, tmp, iterVal, nextVal);
    _curBlock->insert(inst);
    iterVal = inst;
  }
  return std::make_any<ir::Value *>(iterVal);
}

std::any ASTVisitor::visitAdditiveExp(SysYParser::AdditiveExpContext *ctx) {
  auto iterVal = std::any_cast<ir::Value *>(
      ASTVisitor::visitMultiplicativeExp(ctx->multiplicativeExp(0)));
  for (int i = 1; i < ctx->multiplicativeExp().size(); i++) {
    auto nextVal = std::any_cast<ir::Value *>(
        ASTVisitor::visitMultiplicativeExp(ctx->multiplicativeExp(i)));
    ir::Type *targetType = ASTVisitor::automaticTypePromotion(
        iterVal->getType(), nextVal->getType());
    iterVal = typeConversion(iterVal, targetType);
    nextVal = typeConversion(nextVal, targetType);
    auto const number1 = dynamic_cast<ir::ConstantNumber *>(iterVal);
    auto const number2 = dynamic_cast<ir::ConstantNumber *>(nextVal);
    auto txt = ctx->children[i * 2 - 1]->getText();
    if (number1 != nullptr && number2 != nullptr) {
      if (txt == "+") {
        iterVal = number1->add(number2);
      } else if (txt == "-") {
        iterVal = number1->sub(number2);
      } else
        throw std::runtime_error("Invalid operation: " + txt);
      continue;
    }
    ir::BinaryInst::Op op;
    if (txt == "+") {
      if (targetType == ir::BasicType::I32) {
        op = ir::BinaryInst::ADD;
      } else if (targetType == ir::BasicType::FLOAT) {
        op = ir::BinaryInst::FADD;
      } else {
        throw std::runtime_error("Invalid type" + targetType->str());
      }
    } else if (txt == "-") {
      if (targetType == ir::BasicType::I32) {
        op = ir::BinaryInst::SUB;
      } else if (targetType == ir::BasicType::FLOAT) {
        op = ir::BinaryInst::FSUB;
      } else {
        throw std::runtime_error("Invalid type" + targetType->str());
      }
    } else {
      throw std::runtime_error("Invalid operation: " +
                               ctx->children[1]->getText());
    }
    auto inst = new ir::BinaryInst(_curBlock, op, iterVal, nextVal);

    _curBlock->insert(inst);
    iterVal = inst;
  }
  return std::make_any<ir::Value *>(iterVal);
}

std::any ASTVisitor::visitRelationalExp(SysYParser::RelationalExpContext *ctx) {
  auto *iterVal = std::any_cast<ir::Value *>(
      ASTVisitor::visitAdditiveExp(ctx->additiveExp(0)));
  for (int i = 1; i < ctx->additiveExp().size(); i++) {
    auto *nextVal = std::any_cast<ir::Value *>(
        ASTVisitor::visitAdditiveExp(ctx->additiveExp(i)));
    ir::Type *targetType = ASTVisitor::automaticTypePromotion(
        iterVal->getType(), nextVal->getType());
    iterVal = typeConversion(iterVal, targetType);
    nextVal = typeConversion(nextVal, targetType);
    ir::Instruction *inst;
    auto txt = ctx->children[i * 2 - 1]->getText();
    if (txt == "<") {
      if (targetType == ir::BasicType::I32) {
        inst = new ir::ICmpInst(_curBlock, ir::ICmpInst::SLT, iterVal, nextVal);
      } else if (targetType == ir::BasicType::FLOAT) {
        inst = new ir::FCmpInst(_curBlock, ir::FCmpInst::OLT, iterVal, nextVal);
      } else {
        throw std::runtime_error("Invalid type" + targetType->str());
      }
    } else if (txt == ">") {
      if (targetType == ir::BasicType::I32) {
        inst = new ir::ICmpInst(_curBlock, ir::ICmpInst::SGT, iterVal, nextVal);
      } else if (targetType == ir::BasicType::FLOAT) {
        inst = new ir::FCmpInst(_curBlock, ir::FCmpInst::OGT, iterVal, nextVal);
      } else {
        throw std::runtime_error("Invalid type" + targetType->str());
      }
    } else if (txt == "<=") {
      if (targetType == ir::BasicType::I32) {
        inst = new ir::ICmpInst(_curBlock, ir::ICmpInst::SLE, iterVal, nextVal);
      } else if (targetType == ir::BasicType::FLOAT) {
        inst = new ir::FCmpInst(_curBlock, ir::FCmpInst::OLE, iterVal, nextVal);
      } else {
        throw std::runtime_error("Invalid type" + targetType->str());
      }
    } else if (txt == ">=") {
      if (targetType == ir::BasicType::I32) {
        inst = new ir::ICmpInst(_curBlock, ir::ICmpInst::SGE, iterVal, nextVal);
      } else if (targetType == ir::BasicType::FLOAT) {
        inst = new ir::FCmpInst(_curBlock, ir::FCmpInst::OGE, iterVal, nextVal);
      } else {
        throw std::runtime_error("Invalid type" + targetType->str());
      }
    } else {
      throw std::runtime_error("Invalid operation: " +
                               ctx->children[1]->getText());
    }
    _curBlock->insert(inst);
    iterVal = inst;
  }
  return std::make_any<ir::Value *>(iterVal);
}

std::any ASTVisitor::visitEqualityExp(SysYParser::EqualityExpContext *ctx) {
  auto *iterVal = std::any_cast<ir::Value *>(
      ASTVisitor::visitRelationalExp(ctx->relationalExp(0)));
  for (int i = 1; i < ctx->relationalExp().size(); i++) {
    auto *nextVal = std::any_cast<ir::Value *>(
        ASTVisitor::visitRelationalExp(ctx->relationalExp(i)));
    ir::Type *targetType = ASTVisitor::automaticTypePromotion(
        iterVal->getType(), nextVal->getType());
    iterVal = typeConversion(iterVal, targetType);
    nextVal = typeConversion(nextVal, targetType);
    ir::Instruction *inst;
    auto txt = ctx->children[i * 2 - 1]->getText();
    if (txt == "==") {
      if (targetType == ir::BasicType::I32) {
        inst = new ir::ICmpInst(_curBlock, ir::ICmpInst::EQ, iterVal, nextVal);
      } else if (targetType == ir::BasicType::FLOAT) {
        inst = new ir::FCmpInst(_curBlock, ir::FCmpInst::OEQ, iterVal, nextVal);
      } else {
        throw std::runtime_error("Invalid type" + targetType->str());
      }
    } else if (txt == "!=") {
      if (targetType == ir::BasicType::I32) {
        inst = new ir::ICmpInst(_curBlock, ir::ICmpInst::NE, iterVal, nextVal);
      } else if (targetType == ir::BasicType::FLOAT) {
        inst = new ir::FCmpInst(_curBlock, ir::FCmpInst::UNE, iterVal, nextVal);
      } else {
        throw std::runtime_error("Invalid type" + targetType->str());
      }
    } else {
      throw std::runtime_error("Invalid operation: " +
                               ctx->children[1]->getText());
    }
    _curBlock->insert(inst);
    iterVal = inst;
  }
  return std::make_any<ir::Value *>(iterVal);
}

std::any ASTVisitor::visitLandExp(SysYParser::LandExpContext *ctx) {
  std::vector<ir::BasicBlock *> blocks;
  blocks.push_back(_curBlock);
  for (int i = 0; i < ctx->equalityExp().size() - 1; i++) {
    auto *block = new ir::BasicBlock(_curFunc);
    _curFunc->insertBlock(blocks.back(), block);
    blocks.push_back(block);
  }
  ir::BasicBlock *trueBlock = this->_trueBlock;
  ir::BasicBlock *falseBlock = this->_falseBlock;
  for (int i = 0; i < ctx->equalityExp().size() - 1; i++) {
    this->_curBlock = blocks[i];
    this->_trueBlock = blocks[i + 1];
    this->_falseBlock = falseBlock;
    auto *value = std::any_cast<ir::Value *>(
        ASTVisitor::visitEqualityExp(ctx->equalityExp(i)));
    processValueCond(value);
  }
  this->_curBlock = blocks.back();
  this->_trueBlock = trueBlock;
  this->_falseBlock = falseBlock;
  auto value = std::any_cast<ir::Value *>(
      ASTVisitor::visitEqualityExp(ctx->equalityExp().back()));
  processValueCond(value);
  return std::make_any<ir::Value *>(nullptr);
}

std::any ASTVisitor::visitLorExp(SysYParser::LorExpContext *ctx) {
  std::vector<ir::BasicBlock *> blocks;
  blocks.push_back(_curBlock);
  for (int i = 0; i < ctx->landExp().size() - 1; i++) {
    auto block = new ir::BasicBlock(_curFunc);
    _curFunc->insertBlock(blocks.back(), block);
    blocks.push_back(block);
  }
  ir::BasicBlock *trueBlock = this->_trueBlock;
  ir::BasicBlock *falseBlock = this->_falseBlock;
  for (int i = 0; i < ctx->landExp().size() - 1; i++) {
    this->_curBlock = blocks[i];
    this->_trueBlock = trueBlock;
    this->_falseBlock = blocks[i + 1];
    auto value =
        std::any_cast<ir::Value *>(ASTVisitor::visitLandExp(ctx->landExp(i)));
    processValueCond(value);
  }
  this->_curBlock = blocks.back();
  this->_trueBlock = trueBlock;
  this->_falseBlock = falseBlock;
  auto value = std::any_cast<ir::Value *>(
      ASTVisitor::visitLandExp(ctx->landExp().back()));
  processValueCond(value);
  return std::make_any<ir::Value *>(nullptr);
}

std::any ASTVisitor::visitNumberExp(SysYParser::NumberExpContext *ctx) {
  if (ctx->IntConst()) {
    const auto txt = ctx->IntConst()->getText();
    int value = std::stoi(txt);
    if (txt[0] == '0') {
      if (txt[1] != 'x')
        value = std::stoi(txt, nullptr, 8);
      else
        value = std::stoi(txt.substr(2), nullptr, 16);
    }
    return std::make_any<ir::Value *>(
        new ir::ConstantNumber(model::Number(value)));
  }
  if (ctx->FloatConst()) {
    return std::make_any<ir::Value *>(new ir::ConstantNumber(
        model::Number(std::stof(ctx->FloatConst()->getText()))));
  }
  throw std::runtime_error("Invalid number: " + ctx->getText());
}

std::any ASTVisitor::visitFuncCallExp(SysYParser::FuncCallExpContext *ctx) {
  ir::Function *func =
      _symbolTable->getFunc(ctx->Ident()->getSymbol()->getText());
  std::vector<ir::Value *> args;
  for (int i = 0; i < ctx->additiveExp().size(); i++) {
    SysYParser::AdditiveExpContext *exp = ctx->additiveExp()[i];
    auto arg = std::any_cast<ir::Value *>(ASTVisitor::visitAdditiveExp(exp));
    ir::Type *type;
    if (dynamic_cast<ir::BasicType *>(func->getArgs()[i]->getType()) &&
        func->getArgs()[i]->getType() == ir::BasicType::FLOAT) {
      type = ir::BasicType::FLOAT;
    } else {
      type = ir::BasicType::I32;
    }
    arg = typeConversion(arg, type);
    args.push_back(arg);
  }
  ir::Instruction *inst = new ir::CallInst(_curBlock, func, args);
  _curBlock->insert(inst);
  return std::make_any<ir::Value *>(inst);
}

std::any ASTVisitor::visitArrayVarExp(SysYParser::ArrayVarExpContext *ctx) {
  ir::Value *ptr = _symbolTable->getData(ctx->Ident()->getSymbol()->getText());
  bool isFirstDim = false;
  if (auto arg = dynamic_cast<ir::Argument *>(ptr)) {
    isFirstDim = true;
    ptr = _argToAllocaMap[arg];
    ir::Instruction *inst = new ir::LoadInst(_curBlock, ptr);
    ptr = inst;
    _curBlock->insert(inst);
  }
  for (SysYParser::AdditiveExpContext *dim : ctx->additiveExp()) {
    auto index = std::any_cast<ir::Value *>(ASTVisitor::visitAdditiveExp(dim));
    index = typeConversion(index, ir::BasicType::I32);
    ir::Instruction *inst =
        isFirstDim ? new ir::GetPtrInst(_curBlock, ptr, {index})
                   : new ir::GetPtrInst(
                         _curBlock, ptr,
                         {new ir::ConstantNumber(model::Number(0)), index});
    _curBlock->insert(inst);
    ptr = inst;
    isFirstDim = false;
  }
  if (dynamic_cast<ir::ArrayType *>(ptr->getType()->baseType())) {
    return std::make_any<ir::Value *>(ptr);
  }
  ir::Instruction *inst = new ir::LoadInst(_curBlock, ptr);
  _curBlock->insert(inst);
  return std::make_any<ir::Value *>(inst);
}

std::any ASTVisitor::visitScalarVarExp(SysYParser::ScalarVarExpContext *ctx) {
  ir::Value *ptr = _symbolTable->getData(ctx->Ident()->getSymbol()->getText());
  if (auto arg = dynamic_cast<ir::Argument *>(ptr)) {
    ptr = _argToAllocaMap[arg];
  }
  ir::Type *type = ptr->getType();
  if (auto glob = dynamic_cast<ir::GlobalVariable *>(ptr)) {
    if (glob->isConst() && glob->isSingle()) {
      return std::make_any<ir::Value *>(glob->getValue());
    }
    type = new ir::PointerType(type);
  }
  if (auto arr = dynamic_cast<ir::ArrayType *>(type->baseType())) {
    std::vector<ir::Value *> indices(arr->getArraySizes().size(),
                                     new ir::ConstantNumber(model::Number(0)));
    ir::Instruction *inst = new ir::GetPtrInst(_curBlock, ptr, indices);
    _curBlock->insert(inst);
    return std::make_any<ir::Value *>(inst);
  }
  ir::Instruction *inst = new ir::LoadInst(_curBlock, ptr);
  _curBlock->insert(inst);
  return std::make_any<ir::Value *>(inst);
}

std::any ASTVisitor::visitUnaryExp(SysYParser::UnaryExpContext *ctx) {
  size_t const childCount = ctx->children.size();
  ir::Value *result;
  if (childCount == 2) {
    auto *value =
        std::any_cast<ir::Value *>(ASTVisitor::visitUnaryExp(ctx->unaryExp()));
    auto const txt = ctx->children[0]->getText();
    auto const t = value->getType();
    if (auto number = dynamic_cast<ir::ConstantNumber *>(value)) {
      if (txt == "+") {
        result = number;
      } else if (txt == "-") {
        result = number->neg();
      } else if (txt == "!") {
        result = number->lnot();
      } else {
        throw std::runtime_error("Invalid unary operator: " + txt);
      }
      return std::make_any<ir::Value *>(result);
    }
    if (txt == "+") {
      return std::make_any<ir::Value *>(value);
    } else if (txt == "-") {
      ir::Instruction *inst;
      if (t == ir::BasicType::I1) {
        inst = new ir::SExtInst(_curBlock, ir::BasicType::I32, value);
      } else if (t == ir::BasicType::I32) {
        inst = new ir::BinaryInst(_curBlock, ir::BinaryInst::SUB,
                                      new ir::ConstantNumber(model::Number(0)),
                                      value);
      } else if (t == ir::BasicType::FLOAT) {
        inst = new ir::BinaryInst(
            _curBlock, ir::BinaryInst::FSUB,
            new ir::ConstantNumber(model::Number(0.0f)), value);
      } else {
        throw std::runtime_error("Invalid unary operator: " + txt);
      }
      _curBlock->insert(inst);
      return std::make_any<ir::Value *>(inst);
    } else if (txt == "!") {
      ir::Instruction *inst;
      if (t == ir::BasicType::I1) {
        inst = new ir::BinaryInst(_curBlock, ir::BinaryInst::XOR, value,
                                      new ir::ConstantNumber(true));
      } else if (t == ir::BasicType::I32) {
        inst = new ir::ICmpInst(_curBlock, ir::ICmpInst::EQ, value,
                                new ir::ConstantNumber(model::Number(0)));
      } else if (t == ir::BasicType::FLOAT) {
        inst = new ir::FCmpInst(_curBlock, ir::FCmpInst::OEQ, value,
                                new ir::ConstantNumber(model::Number(0.0f)));
      } else {
        throw std::runtime_error("Invalid unary operator: " + txt);
      }
      _curBlock->insert(inst);
      return std::make_any<ir::Value *>(inst);
    } else {
      throw std::runtime_error("Invalid unary operator: " + txt);
    }
  } else if (childCount == 3) {
    return visitAdditiveExp(ctx->additiveExp());
  } else {
    auto res = SysYBaseVisitor::visitUnaryExp(ctx);
    return res;
  }
}

std::any ASTVisitor::visitLVal(SysYParser::LValContext *ctx) {
  ir::Value *ptr = _symbolTable->getData(ctx->Ident()->getSymbol()->getText());
  bool isArg = false;
  if (auto arg = dynamic_cast<ir::Argument *>(ptr)) {
    isArg = true;
    ptr = _argToAllocaMap[arg];
  }
  if (ctx->additiveExp().empty()) {
    return std::make_any<ir::Value *>(ptr);
  }
  auto pType = dynamic_cast<ir::PointerType *>(ptr->getType());
  if (pType) {
    if (dynamic_cast<ir::PointerType *>(pType->baseType())) {
      ir::Instruction *inst = new ir::LoadInst(_curBlock, ptr);
      ptr = inst;
      _curBlock->insert(inst);
    }
  }

  std::vector<SysYParser::AdditiveExpContext *> ctxs = ctx->additiveExp();
  std::vector<ir::Value *> dims;
  std::transform(ctxs.begin(), ctxs.end(), std::back_inserter(dims),
                 [&](SysYParser::AdditiveExpContext *ctx_) {
                   return std::any_cast<ir::Value *>(
                       ASTVisitor::visitAdditiveExp(ctx_));
                 });
  bool isFirst = true;
  for (ir::Value *dim : dims) {
    ir::Instruction *inst;
    if (isArg && isFirst) {
      inst = new ir::GetPtrInst(_curBlock, ptr, {dim});
    } else {
      inst = new ir::GetPtrInst(
          _curBlock, ptr, {new ir::ConstantNumber(model::Number(0)), dim});
    }
    _curBlock->insert(inst);
    ptr = inst;
    isFirst = false;
  }
  return std::make_any<ir::Value *>(ptr);
}

} // namespace parser