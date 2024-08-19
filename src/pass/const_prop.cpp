#include "pass.hpp"

namespace pass {

bool ConstProp::onFunction(ir::Function *function) {
  if (function->isDeclare()) {
    return false;
  }
  bool changed = false;
  for (const auto block : *function) {
    for (int i = 0; i < block->size(); i++) {
      const auto inst = block->get(i);
      if (const auto binaryOperator =
              dynamic_cast<ir::BinaryInst *>(inst)) {
        const auto value1 = dynamic_cast<ir::ConstantNumber *>(
            binaryOperator->getOperand<ir::Value>(0));
        if (value1) {
          const auto value2 = dynamic_cast<ir::ConstantNumber *>(
              binaryOperator->getOperand<ir::Value>(1));
          if (value2) {
            ir::ConstantNumber *newValue = nullptr;
            switch (binaryOperator->op) {
            case ir::BinaryInst::ADD:
            case ir::BinaryInst::FADD:
              newValue = value1->add(value2);
              break;
            case ir::BinaryInst::SUB:
            case ir::BinaryInst::FSUB:
              newValue = value1->sub(value2);
              break;
            case ir::BinaryInst::MUL:
            case ir::BinaryInst::FMUL:
              newValue = value1->mul(value2);
              break;
            case ir::BinaryInst::SDIV:
            case ir::BinaryInst::FDIV:
              newValue = value1->div(value2);
              break;
            case ir::BinaryInst::SREM:
              newValue = value1->rem(value2);
              break;
            case ir::BinaryInst::XOR:
              newValue = value1->exor(value2);
            }
            if (!newValue) {
              throw std::runtime_error("Unsupported binary operator in "
                                       "ConstPropPass::runOnFunction");
            }
            binaryOperator->replaceAllUseAs(newValue);
            block->erase(i);
            inst->clear();
            i--;
            changed = true;
          }
        }
      } else if (const auto cmpInst = dynamic_cast<ir::CmpInst *>(inst)) {
        const auto value1 = dynamic_cast<ir::ConstantNumber *>(
            cmpInst->getOperand<ir::Value>(0));
        if (value1) {
          const auto value2 = dynamic_cast<ir::ConstantNumber *>(
              cmpInst->getOperand<ir::Value>(1));
          if (value2) {
            ir::ConstantNumber *newValue = nullptr;
            switch (cmpInst->getCond()) {
            case ir::CmpInst::EQ:
            case ir::CmpInst::OEQ:
              newValue = value1->eq(value2);
              break;
            case ir::CmpInst::NE:
            case ir::CmpInst::UNE:
              newValue = value1->ne(value2);
              break;
            case ir::CmpInst::SGE:
            case ir::CmpInst::OGE:
              newValue = value1->ge(value2);
              break;
            case ir::CmpInst::SGT:
            case ir::CmpInst::OGT:
              newValue = value1->gt(value2);
              break;
            case ir::CmpInst::SLE:
            case ir::CmpInst::OLE:
              newValue = value1->le(value2);
              break;
            case ir::CmpInst::SLT:
            case ir::CmpInst::OLT:
              newValue = value1->lt(value2);
              break;
            }
            if (!newValue) {
              throw std::runtime_error("Unsupported cmp instruction in "
                                       "ConstPropPass::runOnFunction");
            }
            cmpInst->replaceAllUseAs(newValue);
            block->erase(i);
            inst->clear();
            i--;
            changed = true;
          }
        }
      }
    }
  }
  return changed;
}

} // namespace pass