#include "parser/ast_visitor.hpp"

namespace parser {

std::any
ASTVisitor::visitMultiplicativeExp(SysYParser::MultiplicativeExpContext *ctx) {
  ir::Value *iterVal =
      std::any_cast<ir::Value *>(ASTVisitor::visitUnaryExp(ctx->unaryExp(0)));
  for (int i = 1; i < ctx->unaryExp().size(); i++) {
    ir::Value *nextVal =
        std::any_cast<ir::Value *>(ASTVisitor::visitUnaryExp(ctx->unaryExp(i)));
    ir::Type *targetType = ASTVisitor::automaticTypePromotion(
        iterVal->getType(), nextVal->getType());
    auto number1 = dynamic_cast<ir::ConstantNumber *>(iterVal);
    auto number2 = dynamic_cast<ir::ConstantNumber *>(nextVal);
    if (number1 && number2) {
      if (ctx->children[i * 2 - 1]->getText() == "*") {
        iterVal = number1->mul(number2);
      } else if (ctx->children[i * 2 - 1]->getText() == "/") {
        iterVal = number1->div(number2);
      } else if (ctx->children[i * 2 - 1]->getText() == "%") {
        iterVal = number1->rem(number2);
      } else {
        throw std::runtime_error("Invalid operation: " +
                                 ctx->children[i * 2 - 1]->getText());
      }
      continue;
    }
    ir::BinaryOperator::Op tmp;
    if (ctx->children[i * 2 - 1]->getText() == "*") {
      if (targetType == ir::BasicType::I32) {
        tmp = ir::BinaryOperator::MUL;
      } else if (targetType == ir::BasicType::FLOAT) {
        tmp = ir::BinaryOperator::FMUL;
      } else {
        throw std::runtime_error("Invalid type" + targetType->toString());
      }
    } else if (ctx->children[i * 2 - 1]->getText() == "/") {
      if (targetType == ir::BasicType::I32) {
        tmp = ir::BinaryOperator::SDIV;
      } else if (targetType == ir::BasicType::FLOAT) {
        tmp = ir::BinaryOperator::FDIV;
      } else {
        throw std::runtime_error("Invalid type" + targetType->toString());
      }
    } else if (ctx->children[i * 2 - 1]->getText() == "%") {
      tmp = ir::BinaryOperator::SREM;
    } else {
      throw std::runtime_error("Invalid operation: " +
                               ctx->children[1]->getText());
    }
    ir::Instruction *inst =
        new ir::BinaryOperator(_curBlock, tmp, iterVal, nextVal);
    _curBlock->add(inst);
    iterVal = inst;
  }
  return iterVal;
}

std::any ASTVisitor::visitAdditiveExp(SysYParser::AdditiveExpContext *ctx) {
  ir::Value *iterVal = std::any_cast<ir::Value *>(
      ASTVisitor::visitMultiplicativeExp(ctx->multiplicativeExp(0)));
  for (int i = 1; i < ctx->multiplicativeExp().size(); i++) {
    ir::Value *nextVal = std::any_cast<ir::Value *>(
        ASTVisitor::visitMultiplicativeExp(ctx->multiplicativeExp(i)));
    ir::Type *targetType = ASTVisitor::automaticTypePromotion(
        iterVal->getType(), nextVal->getType());
    iterVal = typeConversion(iterVal, targetType);
    nextVal = typeConversion(nextVal, targetType);
    auto number1 = dynamic_cast<ir::ConstantNumber *>(iterVal);
    auto number2 = dynamic_cast<ir::ConstantNumber *>(nextVal);
    if (number1 && number2) {
      if (ctx->children[i * 2 - 1]->getText() == "+") {
        iterVal = number1->add(number2);
      } else if (ctx->children[i * 2 - 1]->getText() == "-") {
        iterVal = number1->sub(number2);
      } else {
        throw std::runtime_error("Invalid operation: " +
                                 ctx->children[i * 2 - 1]->getText());
      }
      continue;
    }
    ir::BinaryOperator::Op tmp;
    if (ctx->children[i * 2 - 1]->getText() == "+") {
      if (targetType == ir::BasicType::I32) {
        tmp = ir::BinaryOperator::ADD;
      } else if (targetType == ir::BasicType::FLOAT) {
        tmp = ir::BinaryOperator::FADD;
      } else {
        throw std::runtime_error("Invalid type" + targetType->toString());
      }
    } else if (ctx->children[i * 2 - 1]->getText() == "-") {
      if (targetType == ir::BasicType::I32) {
        tmp = ir::BinaryOperator::SUB;
      } else if (targetType == ir::BasicType::FLOAT) {
        tmp = ir::BinaryOperator::FSUB;
      } else {
        throw std::runtime_error("Invalid type" + targetType->toString());
      }
    } else {
      throw std::runtime_error("Invalid operation: " +
                               ctx->children[1]->getText());
    }
    ir::Instruction *inst =
        new ir::BinaryOperator(_curBlock, tmp, iterVal, nextVal);
    _curBlock->add(inst);
    iterVal = inst;
  }
  return iterVal;
}

std::any ASTVisitor::visitRelationalExp(SysYParser::RelationalExpContext *ctx) {
  ir::Value *iterVal = std::any_cast<ir::Value *>(
      ASTVisitor::visitAdditiveExp(ctx->additiveExp(0)));
  for (int i = 1; i < ctx->additiveExp().size(); i++) {
    ir::Value *nextVal = std::any_cast<ir::Value *>(
        ASTVisitor::visitAdditiveExp(ctx->additiveExp(i)));
    ir::Type *targetType = ASTVisitor::automaticTypePromotion(
        iterVal->getType(), nextVal->getType());
    iterVal = typeConversion(iterVal, targetType);
    nextVal = typeConversion(nextVal, targetType);
    ir::Instruction *inst;
    if (ctx->children[i * 2 - 1]->getText() == "<") {
      if (targetType == ir::BasicType::I32) {
        inst = new ir::ICmpInst(_curBlock, ir::ICmpInst::SLT, iterVal, nextVal);
      } else if (targetType == ir::BasicType::FLOAT) {
        inst = new ir::FCmpInst(_curBlock, ir::FCmpInst::OLT, iterVal, nextVal);
      } else {
        throw std::runtime_error("Invalid type" + targetType->toString());
      }
    } else if (ctx->children[i * 2 - 1]->getText() == ">") {
      if (targetType == ir::BasicType::I32) {
        inst = new ir::ICmpInst(_curBlock, ir::ICmpInst::SGT, iterVal, nextVal);
      } else if (targetType == ir::BasicType::FLOAT) {
        inst = new ir::FCmpInst(_curBlock, ir::FCmpInst::OGT, iterVal, nextVal);
      } else {
        throw std::runtime_error("Invalid type" + targetType->toString());
      }
    } else if (ctx->children[i * 2 - 1]->getText() == "<=") {
      if (targetType == ir::BasicType::I32) {
        inst = new ir::ICmpInst(_curBlock, ir::ICmpInst::SLE, iterVal, nextVal);
      } else if (targetType == ir::BasicType::FLOAT) {
        inst = new ir::FCmpInst(_curBlock, ir::FCmpInst::OLE, iterVal, nextVal);
      } else {
        throw std::runtime_error("Invalid type" + targetType->toString());
      }
    } else if (ctx->children[i * 2 - 1]->getText() == ">=") {
      if (targetType == ir::BasicType::I32) {
        inst = new ir::ICmpInst(_curBlock, ir::ICmpInst::SGE, iterVal, nextVal);
      } else if (targetType == ir::BasicType::FLOAT) {
        inst = new ir::FCmpInst(_curBlock, ir::FCmpInst::OGE, iterVal, nextVal);
      } else {
        throw std::runtime_error("Invalid type" + targetType->toString());
      }
    } else {
      throw std::runtime_error("Invalid operation: " +
                               ctx->children[1]->getText());
    }
    _curBlock->add(inst);
    iterVal = inst;
  }
  return iterVal;
}

std::any ASTVisitor::visitEqualityExp(SysYParser::EqualityExpContext *ctx) {
  ir::Value *iterVal = std::any_cast<ir::Value *>(
      ASTVisitor::visitRelationalExp(ctx->relationalExp(0)));
  for (int i = 1; i < ctx->relationalExp().size(); i++) {
    ir::Value *nextVal = std::any_cast<ir::Value *>(
        ASTVisitor::visitRelationalExp(ctx->relationalExp(i)));
    ir::Type *targetType = ASTVisitor::automaticTypePromotion(
        iterVal->getType(), nextVal->getType());
    iterVal = typeConversion(iterVal, targetType);
    nextVal = typeConversion(nextVal, targetType);
    ir::Instruction *inst;
    if (ctx->children[i * 2 - 1]->getText() == "==") {
      if (targetType == ir::BasicType::I32) {
        inst = new ir::ICmpInst(_curBlock, ir::ICmpInst::EQ, iterVal, nextVal);
      } else if (targetType == ir::BasicType::FLOAT) {
        inst = new ir::FCmpInst(_curBlock, ir::FCmpInst::OEQ, iterVal, nextVal);
      } else {
        throw std::runtime_error("Invalid type" + targetType->toString());
      }
    } else if (ctx->children[i * 2 - 1]->getText() == "!=") {
      if (targetType == ir::BasicType::I32) {
        inst = new ir::ICmpInst(_curBlock, ir::ICmpInst::NE, iterVal, nextVal);
      } else if (targetType == ir::BasicType::FLOAT) {
        inst = new ir::FCmpInst(_curBlock, ir::FCmpInst::UNE, iterVal, nextVal);
      } else {
        throw std::runtime_error("Invalid type" + targetType->toString());
      }
    } else {
      throw std::runtime_error("Invalid operation: " +
                               ctx->children[1]->getText());
    }
    _curBlock->add(inst);
    iterVal = inst;
  }
  return iterVal;
}

std::any ASTVisitor::visitLandExp(SysYParser::LandExpContext *ctx) {
  std::vector<ir::BasicBlock *> blocks;
  blocks.push_back(_curBlock);
  for (int i = 0; i < ctx->equalityExp().size() - 1; i++) {
    ir::BasicBlock *block = new ir::BasicBlock(_curFunc);
    _curFunc->insertAfter(blocks.back(), block);
    blocks.push_back(block);
  }
  ir::BasicBlock *trueBlock = this->_trueBlock;
  ir::BasicBlock *falseBlock = this->_falseBlock;
  for (int i = 0; i < ctx->equalityExp().size() - 1; i++) {
    this->_curBlock = blocks[i];
    this->_trueBlock = blocks[i + 1];
    this->_falseBlock = falseBlock;
    ir::Value *value = std::any_cast<ir::Value *>(
        ASTVisitor::visitEqualityExp(ctx->equalityExp(i)));
    processValueCond(value);
  }
  this->_curBlock = blocks.back();
  this->_trueBlock = trueBlock;
  this->_falseBlock = falseBlock;
  ir::Value *value = std::any_cast<ir::Value *>(
      ASTVisitor::visitEqualityExp(ctx->equalityExp().back()));
  processValueCond(value);
  return nullptr;
}

std::any ASTVisitor::visitLorExp(SysYParser::LorExpContext *ctx) {
  std::vector<ir::BasicBlock *> blocks;
  blocks.push_back(_curBlock);
  for (int i = 0; i < ctx->landExp().size() - 1; i++) {
    ir::BasicBlock *block = new ir::BasicBlock(_curFunc);
    _curFunc->insertAfter(blocks.back(), block);
    blocks.push_back(block);
  }
  ir::BasicBlock *trueBlock = this->_trueBlock;
  ir::BasicBlock *falseBlock = this->_falseBlock;
  for (int i = 0; i < ctx->landExp().size() - 1; i++) {
    this->_curBlock = blocks[i];
    this->_trueBlock = trueBlock;
    this->_falseBlock = blocks[i + 1];
    ir::Value *value =
        std::any_cast<ir::Value *>(ASTVisitor::visitLandExp(ctx->landExp(i)));
    processValueCond(value);
  }
  this->_curBlock = blocks.back();
  this->_trueBlock = trueBlock;
  this->_falseBlock = falseBlock;
  ir::Value *value = std::any_cast<ir::Value *>(
      ASTVisitor::visitLandExp(ctx->landExp().back()));
  processValueCond(value);
  return nullptr;
}



} // namespace parser