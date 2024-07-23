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
        throw new std::runtime_error("Invalid operation: " +
                                     ctx->children[i * 2 - 1]->getText());
      }
      continue;
    }
    // WIP
  }
}

} // namespace parser