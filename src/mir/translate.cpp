#include "mir/translate.hpp"

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

void MIRBinTranslator::transModRegImm(std::vector<MIR *> irs, reg::VReg *target,
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
                               MIROpUtil::leadingZeros(imm) + 10));
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
    auto midReg = new reg::VReg(ir::BasicType::I32);
    MIROpUtil::loadImmToReg(irs, midReg, value->intValue());
    irs.push_back(new RrrMIR(RrrMIR::XOR, target, reg, midReg));
    break;
  default:
    throw std::runtime_error(
        "unsupported binary operator in MIRBinTranslator::transBinRegImm");
  }
}

void MIRBinTranslator::transBinRegReg(
    std::vector<MIR *> &irs,
    const std::unordered_map<ir::Instruction *, reg::VReg *> &instRegMap,
    ir::BinaryOperator *binOp, reg::VReg *reg1, reg::VReg *reg2) {
  auto target = instRegMap.at(binOp);
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

} // namespace mir