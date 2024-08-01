#include "translate.hpp"

namespace mir {

void MIRBinTranslator::transAddRegImmF(std::vector<MIR *> &irs,
                                       reg::VReg *target, reg::VReg *vsrc,
                                       float imm) {
  auto midReg = new reg::VReg(ir::BasicType::FLOAT);
  MIROpUtil::loadImmToReg(irs, midReg, imm);
  transAddRegRegF(irs, target, vsrc, midReg);
}

void MIRBinTranslator::transDivImmRegF(std::vector<MIR *> &irs,
                                       reg::VReg *target, float imm,
                                       reg::VReg *vsrc) {

  auto midReg = new reg::VReg(ir::BasicType::FLOAT);
  MIROpUtil::loadImmToReg(irs, midReg, imm);
  transDivRegRegF(irs, target, midReg, vsrc);
}

void MIRBinTranslator::transDivImmRegI(std::vector<MIR *> &irs,
                                       reg::VReg *target, int imm,
                                       reg::VReg *vsrc) {
  auto midReg = new reg::VReg(ir::BasicType::I32);
  MIROpUtil::loadImmToReg(irs, midReg, imm);
  transDivRegRegI(irs, target, midReg, vsrc);
}

void MIRBinTranslator::transDivRegImmF(std::vector<MIR *> &irs,
                                       reg::VReg *target, reg::VReg *vsrc,
                                       float imm) {
  auto midReg = new reg::VReg(ir::BasicType::FLOAT);
  MIROpUtil::loadImmToReg(irs, midReg, imm);
  transDivRegRegF(irs, target, vsrc, midReg);
}

void MIRBinTranslator::transDivRegImmI(std::vector<MIR *> &irs,
                                       reg::VReg *target, reg::VReg *vsrc,
                                       int imm) {
  if (imm == 1) {
    irs.push_back(new RrMIR(RrMIR::MV, target, vsrc));
    return;
  }
  if (imm == -1) {
    irs.push_back(new RrMIR(RrMIR::NEG, target, vsrc));
    return;
  }
  int div = imm;
  bool isPos = true;
  if (div < 0) {
    isPos = false;
    div = std::abs(div);
  }
  int shift = 0;
  while (1L << (shift + 32) <= (0x7fffffffL - 0x80000000L % div) *
                                   (div - (1L << (shift + 32)) % div)) {
    shift++;
  }
  int magic = static_cast<int>((1L << (shift + 32)) / div + 1);
  auto midReg1 = new reg::VReg(ir::BasicType::I32);
  auto midReg2 = new reg::VReg(ir::BasicType::I32);
  auto midReg3 = new reg::VReg(ir::BasicType::I32);
  auto midReg4 = new reg::VReg(ir::BasicType::I32);
  MIROpUtil::loadImmToReg(irs, midReg1, magic);
  if (magic >= 0) {
    irs.push_back(new RrrMIR(RrrMIR::MUL, midReg2, vsrc, midReg1));
    irs.push_back(new RriMIR(RriMIR::SRLI, midReg2, midReg2, 32));
  } else {
    irs.push_back(new RrrMIR(RrrMIR::MUL, midReg2, vsrc, midReg1));
    irs.push_back(new RriMIR(RriMIR::SRLI, midReg2, midReg2, 32));
    irs.push_back(new RrrMIR(RrrMIR::ADD, midReg2, midReg2, vsrc));
  }
  if (shift != 0) {
    irs.push_back(new RriMIR(RriMIR::SRAIW, midReg3, midReg2, shift));
  } else {
    midReg3 = midReg2;
  }
  if (isPos) {
    irs.push_back(new RriMIR(RriMIR::SRLIW, midReg4, vsrc, 31));
    irs.push_back(new RrrMIR(RrrMIR::ADDW, target, midReg3, midReg4));
  } else {
    irs.push_back(new RriMIR(RriMIR::SRAIW, midReg4, vsrc, 31));
    irs.push_back(new RrrMIR(RrrMIR::SUBW, target, midReg3, midReg4));
  }
}

void MIRBinTranslator::transModImmReg(std::vector<MIR *> &irs,
                                      reg::VReg *target, int imm,
                                      reg::VReg *vsrc) {
  auto midReg = new reg::VReg(ir::BasicType::I32);
  MIROpUtil::loadImmToReg(irs, midReg, imm);
  transModRegReg(irs, target, midReg, vsrc);
}

void MIRBinTranslator::transModRegImm(std::vector<MIR *> &irs, reg::VReg *target,
                                      reg::VReg *vsrc, int imm) {
  if (imm == 1) {
    irs.push_back(new RrMIR(RrMIR::MV, target, vsrc));
    return;
  }
  if (MIROpUtil::bitcount(imm) == 1) {
    auto midReg1 = new reg::VReg(ir::BasicType::I32);
    if (imm == 2) {
      irs.push_back(new RriMIR(RriMIR::SRLIW, midReg1, vsrc, 31));
    } else {
      irs.push_back(new RriMIR(RriMIR::SRAIW, midReg1, vsrc, 31));
      irs.push_back(new RriMIR(RriMIR::SRLIW, midReg1, midReg1,
                               MIROpUtil::leadingZeros(imm) + 1));
    }
    irs.push_back(new RrrMIR(RrrMIR::ADD, midReg1, midReg1, vsrc));
    if (std::abs(imm) < 2048) {
      irs.push_back(new RriMIR(RriMIR::ANDI, midReg1, midReg1, -imm));
    } else {
      auto midReg2 = new reg::VReg(ir::BasicType::I32);
      irs.push_back(new LiMIR(midReg1, -imm));
      irs.push_back(new RrrMIR(RrrMIR::AND, midReg1, midReg1, midReg2));
    }
    irs.push_back(new RrrMIR(RrrMIR::SUBW, target, vsrc, midReg1));
    return;
  }
  auto midReg1 = new reg::VReg(ir::BasicType::I32);
  auto midReg2 = new reg::VReg(ir::BasicType::I32);
  transDivRegImmI(irs, midReg1, vsrc, imm);
  transMulRegImmI(irs, midReg2, midReg1, imm);
  irs.push_back(new RrrMIR(RrrMIR::SUBW, target, vsrc, midReg2));
}

void MIRBinTranslator::transMulRegImmF(std::vector<MIR *> &irs,
                                       reg::VReg *target, reg::VReg *vsrc,
                                       float imm) {
  auto midReg = new reg::VReg(ir::BasicType::FLOAT);
  MIROpUtil::loadImmToReg(irs, midReg, imm);
  transMulRegRegF(irs, target, vsrc, midReg);
}

void MIRBinTranslator::transMulRegImmI(std::vector<MIR *> &irs,
                                       reg::VReg *target, reg::VReg *vsrc,
                                       int imm) {
  if (imm == 0) {
    MIROpUtil::loadImmToReg(irs, target, 0);
    return;
  }
  if (imm == 1) {
    irs.push_back(new RrMIR(RrMIR::MV, target, vsrc));
    return;
  }
  if (imm == -1) {
    irs.push_back(new RrMIR(RrMIR::NEG, target, vsrc));
    return;
  }
  if (MIROpUtil::bitcount(imm) == 1) {
    irs.push_back(
        new RriMIR(RriMIR::SLLIW, target, vsrc, MIROpUtil::trailingZeros(imm)));
    return;
  }
  if (MIROpUtil::bitcount(imm) == 2 && imm % 2 == 1) {
    auto midReg = new reg::VReg(ir::BasicType::I32);
    irs.push_back(new RriMIR(RriMIR::SLLIW, midReg, vsrc,
                             31 - MIROpUtil::leadingZeros(imm)));
    irs.push_back(new RrrMIR(RrrMIR::ADDW, target, midReg, vsrc));
    return;
  }
  if (MIROpUtil::trailingZeros(imm) == 0 &&
      MIROpUtil::leadingZeros(imm) + MIROpUtil::bitcount(imm) == 32) {
    auto midReg = new reg::VReg(ir::BasicType::I32);
    irs.push_back(new RriMIR(RriMIR::SLLIW, midReg, vsrc,
                             32 - MIROpUtil::leadingZeros(imm)));
    irs.push_back(new RrrMIR(RrrMIR::SUBW, target, midReg, vsrc));
    return;
  }
  auto midReg = new reg::VReg(ir::BasicType::I32);
  MIROpUtil::loadImmToReg(irs, midReg, imm);
  transMulRegRegI(irs, target, vsrc, midReg);
}

void MIRBinTranslator::transSubImmRegF(std::vector<MIR *> &irs,
                                       reg::VReg *target, float imm,
                                       reg::VReg *vsrc) {
  auto midReg = new reg::VReg(ir::BasicType::FLOAT);
  MIROpUtil::loadImmToReg(irs, midReg, imm);
  transSubRegRegF(irs, target, midReg, vsrc);
}

void MIRBinTranslator::transSubImmRegI(std::vector<MIR *> &irs,
                                       reg::VReg *target, int imm,
                                       reg::VReg *vsrc) {
  auto midReg = new reg::VReg(ir::BasicType::I32);
  MIROpUtil::loadImmToReg(irs, midReg, imm);
  transSubRegRegI(irs, target, midReg, vsrc);
}

void MIRBinTranslator::transSubRegImmF(std::vector<MIR *> &irs,
                                       reg::VReg *target, reg::VReg *vsrc,
                                       float imm) {
  auto midReg = new reg::VReg(ir::BasicType::FLOAT);
  MIROpUtil::loadImmToReg(irs, midReg, imm);
  transSubRegRegF(irs, target, vsrc, midReg);
}

void MIRBinTranslator::transSubRegImmI(std::vector<MIR *> &irs,
                                       reg::VReg *target, reg::VReg *vsrc,
                                       int imm) {
  // TODO check here
  if (-imm >= -2048 && -imm < 2048) {
    irs.push_back(new RriMIR(RriMIR::ADDI, target, vsrc, -imm));
    return;
  }
  auto midReg = new reg::VReg(ir::BasicType::I32);
  MIROpUtil::loadImmToReg(irs, midReg, imm);
  transSubRegRegI(irs, target, vsrc, midReg);
}

void MIRBinTranslator::transAddRegImmI(std::vector<MIR *> &irs,
                                       reg::VReg *target, reg::VReg *vsrc,
                                       int imm) {
  if (imm >= -2048 && imm < 2048) {
    irs.push_back(new RriMIR(RriMIR::ADDI, target, vsrc, imm));
    return;
  }
  auto midReg = new reg::VReg(ir::BasicType::I32);
  MIROpUtil::loadImmToReg(irs, midReg, imm);
  transAddRegRegI(irs, target, vsrc, midReg);
}

void MIRBinTranslator::transBinImmReg(
    std::vector<MIR *> &irs,
    const std::unordered_map<ir::Instruction *, reg::VReg *> &instRegMap,
    ir::BinaryOperator *binOp, ir::ConstantNumber *value, reg::VReg *reg) {
  auto target = instRegMap.at(binOp);
  switch (binOp->op) {
  case ir::BinaryOperator::ADD:
    transAddRegImmI(irs, target, reg, value->intValue());
    break;
  case ir::BinaryOperator::FADD:
    transAddRegImmF(irs, target, reg, value->floatValue());
    break;
  case ir::BinaryOperator::SDIV:
    transDivImmRegI(irs, target, value->intValue(), reg);
    break;
  case ir::BinaryOperator::FDIV:
    transDivImmRegF(irs, target, value->floatValue(), reg);
    break;
  case ir::BinaryOperator::SREM:
    transModImmReg(irs, target, value->intValue(), reg);
    break;
  case ir::BinaryOperator::MUL:
    transMulRegImmI(irs, target, reg, value->intValue());
    break;
  case ir::BinaryOperator::FMUL:
    transMulRegImmF(irs, target, reg, value->floatValue());
    break;
  case ir::BinaryOperator::SUB:
    transSubImmRegI(irs, target, value->intValue(), reg);
    break;
  case ir::BinaryOperator::FSUB:
    transSubImmRegF(irs, target, value->floatValue(), reg);
    break;
  default:
    throw std::runtime_error(
        "unsupported binary operator in MIRBinTranslator::transBinImmReg");
  }
}

void MIRBinTranslator::transBinRegImm(
    std::vector<MIR *> &irs,
    const std::unordered_map<ir::Instruction *, reg::VReg *> &instRegMap,
    ir::BinaryOperator *binOp, reg::VReg *reg, ir::ConstantNumber *value) {
  auto target = instRegMap.at(binOp);
  switch (binOp->op) {
  case ir::BinaryOperator::ADD:
    transAddRegImmI(irs, target, reg, value->intValue());
    break;
  case ir::BinaryOperator::FADD:
    transAddRegImmF(irs, target, reg, value->floatValue());
    break;
  case ir::BinaryOperator::SDIV:
    transDivRegImmI(irs, target, reg, value->intValue());
    break;
  case ir::BinaryOperator::FDIV:
    transDivRegImmF(irs, target, reg, value->floatValue());
    break;
  case ir::BinaryOperator::SREM:
    transModRegImm(irs, target, reg, value->intValue());
    break;
  case ir::BinaryOperator::MUL:
    transMulRegImmI(irs, target, reg, value->intValue());
    break;
  case ir::BinaryOperator::FMUL:
    transMulRegImmF(irs, target, reg, value->floatValue());
    break;
  case ir::BinaryOperator::SUB:
    transSubRegImmI(irs, target, reg, value->intValue());
    break;
  case ir::BinaryOperator::FSUB:
    transSubRegImmF(irs, target, reg, value->floatValue());
    break;
  case ir::BinaryOperator::XOR:
    auto const midReg = new reg::VReg(ir::BasicType::I32);
    MIROpUtil::loadImmToReg(irs, midReg, value->intValue());
    irs.push_back(new RrrMIR(RrrMIR::XOR, target, reg, midReg));
    break;
  }
}

void MIRBinTranslator::transBinRegReg(
    std::vector<MIR *> &irs,
    const std::unordered_map<ir::Instruction *, reg::VReg *> &instRegMap,
    ir::BinaryOperator *binOp, reg::VReg *reg1, reg::VReg *reg2) {
  auto const target = instRegMap.at(binOp);
  switch (binOp->op) {
  case ir::BinaryOperator::ADD:
    transAddRegRegI(irs, target, reg1, reg2);
    break;
  case ir::BinaryOperator::FADD:
    transAddRegRegF(irs, target, reg1, reg2);
    break;
  case ir::BinaryOperator::SDIV:
    transDivRegRegI(irs, target, reg1, reg2);
    break;
  case ir::BinaryOperator::FDIV:
    transDivRegRegF(irs, target, reg1, reg2);
    break;
  case ir::BinaryOperator::SREM:
    transModRegReg(irs, target, reg1, reg2);
    break;
  case ir::BinaryOperator::MUL:
    transMulRegRegI(irs, target, reg1, reg2);
    break;
  case ir::BinaryOperator::FMUL:
    transMulRegRegF(irs, target, reg1, reg2);
    break;
  case ir::BinaryOperator::SUB:
    transSubRegRegI(irs, target, reg1, reg2);
    break;
  case ir::BinaryOperator::FSUB:
    transSubRegRegF(irs, target, reg1, reg2);
    break;
  default:
    throw std::runtime_error(
        "unsupported binary operator in MIRBinTranslator::transBinRegReg");
  }
}

void MIROpTranslator::transBranch(
    std::vector<MIR *> &irs,
    std::unordered_map<ir::Instruction *, reg::VReg *> const &instRegMap,
    ir::BranchInst *branchInst) {
  if (!branchInst->isConditional()) {
    auto const dest = branchInst->getOperand<ir::BasicBlock>(0);
    irs.push_back(new BMIR(BMIR::NUL, nullptr, nullptr, dest));
    return;
  }
  auto const cond = branchInst->getOperand<ir::Value>(0);
  auto const trueBlock = branchInst->getOperand<ir::BasicBlock>(1);
  auto const falseBlock = branchInst->getOperand<ir::BasicBlock>(2);
  reg::VReg *reg;
  if (auto const ir = dynamic_cast<ir::Instruction *>(cond)) {
    reg = instRegMap.at(ir);
  } else if (auto const value = dynamic_cast<ir::ConstantNumber *>(cond)) {
    auto const midReg = new reg::VReg(value->getType());
    if (value->getType() == ir::BasicType::FLOAT) {
      MIROpUtil::loadImmToReg(irs, midReg, value->floatValue());
    } else {
      MIROpUtil::loadImmToReg(irs, midReg, value->intValue());
    }
    reg = midReg;
  } else {
    throw std::runtime_error(
        "unsupported branch condition in MIROpTranslator::transBranch");
  }
  irs.push_back(new BMIR(BMIR::NE, reg, reg::MReg::ZERO, trueBlock));
  irs.push_back(new BMIR(BMIR::NUL, nullptr, nullptr, falseBlock));
}

void MIROpTranslator::transBin(
    std::vector<MIR *> &irs,
    std::unordered_map<ir::Instruction *, reg::VReg *> const &instRegMap,
    std::unordered_map<ir::Argument *, std::pair<bool, int>> const &argOffsets,
    ir::BinaryOperator *binOp) {
  auto const operand1 = binOp->getOperand<ir::Value>(0);
  auto const operand2 = binOp->getOperand<ir::Value>(1);
  reg::VReg *reg1 = nullptr;
  if (auto const arg = dynamic_cast<ir::Argument *>(operand1)) {
    auto const midReg = new reg::VReg(arg->getType() == ir::BasicType::FLOAT
                                          ? ir::BasicType::FLOAT
                                          : ir::BasicType::I32);
    irs.push_back(new LoadItemMIR(argOffsets.at(arg).first
                                      ? LoadItemMIR::PARAM_INNER
                                      : LoadItemMIR::PARAM_OUTER,
                                  midReg, argOffsets.at(arg).second));
    reg1 = midReg;
  } else if (auto const inst = dynamic_cast<ir::Instruction *>(operand1)) {
    reg1 = instRegMap.at(inst);
  }
  reg::VReg *reg2 = nullptr;
  if (auto const arg = dynamic_cast<ir::Argument *>(operand2)) {
    auto const midReg = new reg::VReg(arg->getType() == ir::BasicType::FLOAT
                                          ? ir::BasicType::FLOAT
                                          : ir::BasicType::I32);
    irs.push_back(new LoadItemMIR(argOffsets.at(arg).first
                                      ? LoadItemMIR::PARAM_INNER
                                      : LoadItemMIR::PARAM_OUTER,
                                  midReg, argOffsets.at(arg).second));
    reg2 = midReg;
  } else if (auto const inst = dynamic_cast<ir::Instruction *>(operand2)) {
    reg2 = instRegMap.at(inst);
  }
  if (reg1 && reg2) {
    MIRBinTranslator::transBinRegReg(irs, instRegMap, binOp, reg1, reg2);
    return;
  }
  auto const value2 = dynamic_cast<ir::ConstantNumber *>(operand2);
  if (reg1 && value2) {
    MIRBinTranslator::transBinRegImm(irs, instRegMap, binOp, reg1, value2);
    return;
  }
  auto const value1 = dynamic_cast<ir::ConstantNumber *>(operand1);
  if (reg2 && value1) {
    MIRBinTranslator::transBinImmReg(irs, instRegMap, binOp, value1, reg2);
    return;
  }
  if (value1 && value2) {
    auto const midReg = new reg::VReg(value1->getType());
    auto const type1 = value1->getType();
    if (type1 == ir::BasicType::I32) {
      MIROpUtil::loadImmToReg(irs, midReg, value1->intValue());
    } else if (type1 == ir::BasicType::FLOAT) {
      MIROpUtil::loadImmToReg(irs, midReg, value1->floatValue());
    } else {
      throw std::runtime_error(
          "unsupported constant number type in MIROpTranslator::transBin");
    }
    MIRBinTranslator::transBinRegImm(irs, instRegMap, binOp, midReg, value2);
    return;
  }
  throw std::runtime_error(
      "unsupported binary operator in MIROpTranslator::transBin");
}

int MIROpTranslator::transCall(
    std::vector<MIR *> &irs,
    std::unordered_map<ir::Instruction *, reg::VReg *> const &instRegMap,
    std::unordered_map<ir::Argument *, std::pair<bool, int>> const &argOffsets,
    ir::CallInst *callInst,
    std::unordered_map<ir::AllocaInst *, int> const &localOffsets) {
  auto func = callInst->getOperand<ir::Function>(0);
  std::vector<MIR *> saveCalleeIRs;
  int iSize = 0, fSize = 0;
  for (int i = 1; i < callInst->size(); i++) {
    auto const param = callInst->getOperand<ir::Value>(i);
    if (param->getType() == ir::BasicType::FLOAT) {
      if (fSize < reg::MReg::F_CALLER_REGS.size()) {
        if (auto const arg = dynamic_cast<ir::Argument *>(param)) {
          saveCalleeIRs.push_back(new LoadItemMIR(
              argOffsets.at(arg).first ? LoadItemMIR::PARAM_INNER
                                       : LoadItemMIR::PARAM_OUTER,
              reg::MReg::F_CALLER_REGS.at(fSize), argOffsets.at(arg).second));
        } else if (auto const allocaInst =
                       dynamic_cast<ir::AllocaInst *>(param)) {
          saveCalleeIRs.push_back(new RrMIR(RrMIR::MV,
                                            reg::MReg::F_CALLER_REGS.at(fSize),
                                            instRegMap.at(allocaInst)));
        } else if (auto const inst = dynamic_cast<ir::Instruction *>(param)) {
          saveCalleeIRs.push_back(new RrMIR(RrMIR::MV,
                                            reg::MReg::F_CALLER_REGS.at(fSize),
                                            instRegMap.at(inst)));
        } else if (auto const value =
                       dynamic_cast<ir::ConstantNumber *>(param)) {
          MIROpUtil::loadImmToReg(saveCalleeIRs,
                                  reg::MReg::F_CALLER_REGS.at(fSize),
                                  value->floatValue());
        } else {
          throw std::runtime_error(
              "unsupported parameter type in MIROpTranslator::transCall");
        }
      } else {
        if (auto const arg = dynamic_cast<ir::Argument *>(param)) {
          auto midReg = new reg::VReg(arg->getType());
          irs.push_back(new LoadItemMIR(argOffsets.at(arg).first
                                            ? LoadItemMIR::PARAM_INNER
                                            : LoadItemMIR::PARAM_OUTER,
                                        midReg, argOffsets.at(arg).second));
          irs.push_back(new StoreItemMIR(
              StoreItemMIR::PARAM_CALL, midReg,
              (std::max(iSize -
                            static_cast<int>(reg::MReg::I_CALLER_REGS.size()),
                        0) +
               std::max(fSize -
                            static_cast<int>(reg::MReg::I_CALLER_REGS.size()),
                        0)) *
                  8));
        } else if (auto const inst = dynamic_cast<ir::Instruction *>(param)) {
          irs.push_back(new StoreItemMIR(
              StoreItemMIR::PARAM_CALL, instRegMap.at(inst),
              (std::max(iSize -
                            static_cast<int>(reg::MReg::I_CALLER_REGS.size()),
                        0) +
               std::max(fSize -
                            static_cast<int>(reg::MReg::I_CALLER_REGS.size()),
                        0)) *
                  8));
        } else if (auto const value =
                       dynamic_cast<ir::ConstantNumber *>(param)) {
          auto const midReg = new reg::VReg(ir::BasicType::I32);
          MIROpUtil::loadImmToReg(irs, midReg, value->floatValue());
          irs.push_back(new StoreItemMIR(
              StoreItemMIR::PARAM_CALL, midReg,
              (std::max(iSize -
                            static_cast<int>(reg::MReg::I_CALLER_REGS.size()),
                        0) +
               std::max(fSize -
                            static_cast<int>(reg::MReg::I_CALLER_REGS.size()),
                        0)) *
                  8));
        } else {
          throw std::runtime_error(
              "unsupported parameter type in MIROpTranslator::transCall");
        }
      }
      fSize++;
    } else {
      if (iSize < reg::MReg::I_CALLER_REGS.size()) {
        if (auto const global = dynamic_cast<ir::GlobalVariable *>(param)) {
          saveCalleeIRs.push_back(
              new LlaMIR(reg::MReg::I_CALLER_REGS.at(iSize), global));
        } else if (auto const allocaInst =
                       dynamic_cast<ir::AllocaInst *>(param)) {
          if (dynamic_cast<ir::PointerType *>(allocaInst->getType())) {
            saveCalleeIRs.push_back(
                new AddRegLocalMIR(reg::MReg::I_CALLER_REGS.at(iSize),
                                   localOffsets.at(allocaInst)));
          } else {
            saveCalleeIRs.push_back(
                new RrMIR(RrMIR::MV, reg::MReg::I_CALLER_REGS.at(iSize),
                          instRegMap.at(allocaInst)));
          }
        } else if (auto const arg = dynamic_cast<ir::Argument *>(param)) {
          saveCalleeIRs.push_back(new LoadItemMIR(
              argOffsets.at(arg).first ? LoadItemMIR::PARAM_INNER
                                       : LoadItemMIR::PARAM_OUTER,
              reg::MReg::I_CALLER_REGS.at(iSize), argOffsets.at(arg).second));
        } else if (auto const inst = dynamic_cast<ir::Instruction *>(param)) {
          saveCalleeIRs.push_back(new RrMIR(RrMIR::MV,
                                            reg::MReg::I_CALLER_REGS.at(iSize),
                                            instRegMap.at(inst)));
        } else if (auto const value =
                       dynamic_cast<ir::ConstantNumber *>(param)) {
          MIROpUtil::loadImmToReg(saveCalleeIRs,
                                  reg::MReg::I_CALLER_REGS.at(iSize),
                                  value->intValue());
        } else {
          throw std::runtime_error(
              "unsupported parameter type in MIROpTranslator::transCall");
        }
      } else {
        if (auto const allocaInst = dynamic_cast<ir::AllocaInst *>(param)) {
          auto const midReg = new reg::VReg(ir::BasicType::I32);
          irs.push_back(
              new AddRegLocalMIR(midReg, localOffsets.at(allocaInst)));
          irs.push_back(new StoreItemMIR(
              StoreItemMIR::PARAM_CALL, midReg,
              (std::max(iSize -
                            static_cast<int>(reg::MReg::I_CALLER_REGS.size()),
                        0) +
               std::max(fSize -
                            static_cast<int>(reg::MReg::I_CALLER_REGS.size()),
                        0)) *
                  8));
        } else if (auto const arg = dynamic_cast<ir::Argument *>(param)) {
          auto const midReg = new reg::VReg(arg->getType());
          irs.push_back(new LoadItemMIR(argOffsets.at(arg).first
                                            ? LoadItemMIR::PARAM_INNER
                                            : LoadItemMIR::PARAM_OUTER,
                                        midReg, argOffsets.at(arg).second));
          irs.push_back(new StoreItemMIR(
              StoreItemMIR::PARAM_CALL, midReg,
              (std::max(iSize -
                            static_cast<int>(reg::MReg::I_CALLER_REGS.size()),
                        0) +
               std::max(fSize -
                            static_cast<int>(reg::MReg::I_CALLER_REGS.size()),
                        0)) *
                  8));
        } else if (auto const inst = dynamic_cast<ir::Instruction *>(param)) {
          irs.push_back(new StoreItemMIR(
              StoreItemMIR::PARAM_CALL, instRegMap.at(inst),
              (std::max(iSize -
                            static_cast<int>(reg::MReg::I_CALLER_REGS.size()),
                        0) +
               std::max(fSize -
                            static_cast<int>(reg::MReg::I_CALLER_REGS.size()),
                        0)) *
                  8));
        } else if (auto const value =
                       dynamic_cast<ir::ConstantNumber *>(param)) {
          auto const midReg = new reg::VReg(ir::BasicType::I32);
          MIROpUtil::loadImmToReg(irs, midReg, value->intValue());
          irs.push_back(new StoreItemMIR(
              StoreItemMIR::PARAM_CALL, midReg,
              (std::max(iSize -
                            static_cast<int>(reg::MReg::I_CALLER_REGS.size()),
                        0) +
               std::max(fSize -
                            static_cast<int>(reg::MReg::I_CALLER_REGS.size()),
                        0)) *
                  8));
        } else {
          throw std::runtime_error(
              "unsupported parameter type in MIROpTranslator::transCall");
        }
      }
      iSize++;
    }
  }
  irs.insert(irs.end(), saveCalleeIRs.begin(), saveCalleeIRs.end());
  irs.push_back(new CallMIR(func));
  if (callInst->getType() != ir::BasicType::VOID) {
    auto const target = instRegMap.at(callInst);
    auto const instType = callInst->getType();
    reg::Reg *reg;
    if (instType == ir::BasicType::I32) {
      reg = reg::MReg::A0;
    } else if (instType == ir::BasicType::FLOAT) {
      reg = reg::MReg::FA0;
    } else {
      throw std::runtime_error(
          "unsupported return type in MIROpTranslator::transCall");
    }
    irs.push_back(new RrMIR(RrMIR::MV, target, reg));
  }
  return static_cast<int>(callInst->size()) - 1;
}

} // namespace mir
