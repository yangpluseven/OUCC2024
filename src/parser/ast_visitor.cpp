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
                           new ir::ConstantNumber(model::IntNumber(0)));
      _curBlock->add(inst);
      cond = inst;
    } else if (value->getType() == ir::BasicType::FLOAT) {
      ir::Instruction *inst =
          new ir::FCmpInst(_curBlock, ir::FCmpInst::UNE, value,
                           new ir::ConstantNumber(model::FloatNumber(0.0f)));
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
                           new ir::ConstantNumber(model::IntNumber(0)));
      _curBlock->add(inst);
      return inst;
    } else if (value->getType() == ir::BasicType::FLOAT) {
      ir::Instruction *inst =
          new ir::FCmpInst(_curBlock, ir::FCmpInst::UNE, value,
                           new ir::ConstantNumber(model::FloatNumber(0.0f)));
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
      values[index] = *result->getValue();
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

    _curBlock->add(new ir::CallInst(
        _curBlock, _symbolTable->getFunc("memset"),
        {bitCastInst, new ir::ConstantNumber(model::IntNumber(0)),
         new ir::ConstantNumber(model::IntNumber(x))}));

    for (const auto &entry : exps) {
      auto value = typeConversion(visitAdditiveExp(entry.second), _curType);
      auto ptr = allocaInst;
      for (int j = 0; j.dimensions.size(); j++) {
        int x = 1;
        for (auto k = j + 1; k < dimensions.size(); k++)
          x *= dimensions[k];
        int index = entry.first / x % dimensions[j];
        auto inst = new ir::GetElementPtrInst(
            _curBlock, ptr,
            {new ir::ConstantNumber(model::IntNumber(0)),
             new ir::ConstantNumber(model::IntNumber(index))});
        //WIP
      }
    }
  }

} // namespace parser