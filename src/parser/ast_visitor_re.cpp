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
    ir::BinaryOperator::Op tmp = ir::BinaryOperator::ADD;
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
    if (number1 != nullptr && number2 != nullptr) {
      if (ctx->children[i * 2 - 1]->getText() == "+") {
        iterVal = number1->add(number2);
      } else if (ctx->children[i * 2 - 1]->getText() == "-") {
        iterVal = number1->sub(number2);
      } else
        throw std::runtime_error("Invalid operation: " +
                                 ctx->children[i * 2 - 1]->getText());
      continue;
    }
    auto txt = ctx->children[i * 2 - 1]->getText();
    ir::BinaryOperator::Op op;
    if (txt == "+")
      if (targetType == ir::BasicType::I32)
        op = ir::BinaryOperator::ADD;
      else if (targetType == ir::BasicType::FLOAT)
        op = ir::BinaryOperator::FADD;
      else
        throw std::runtime_error("Invalid type" + targetType->toString());
    else if (txt == "-")
      if (targetType == ir::BasicType::I32)
        op = ir::BinaryOperator::SUB;
      else if (targetType == ir::BasicType::FLOAT)
        op = ir::BinaryOperator::FSUB;
      else
        throw std::runtime_error("Invalid type" + targetType->toString());
    else
      throw std::runtime_error("Invalid operation: " + txt);

    auto inst = new ir::BinaryOperator(_curBlock, op, iterVal, nextVal);

    _curBlock->add(inst);
    iterVal = inst;
  }
  return iterVal;
}
} // namespace parser