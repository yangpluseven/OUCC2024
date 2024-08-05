#include "translate.hpp"

namespace mir {

void MIRBinTranslator::transAddRegImmF(std::vector<MIR *> &irs,
                                       reg::Virtual *target, reg::Virtual *vsrc,
                                       float imm) {
  auto midReg = new reg::Virtual(ir::BasicType::FLOAT);
  MIROpUtil::loadImmToReg(irs, midReg, imm);
  transAddRegRegF(irs, target, vsrc, midReg);
}

void MIRBinTranslator::transDivImmRegF(std::vector<MIR *> &irs,
                                       reg::Virtual *target, float imm,
                                       reg::Virtual *vsrc) {

  auto midReg = new reg::Virtual(ir::BasicType::FLOAT);
  MIROpUtil::loadImmToReg(irs, midReg, imm);
  transDivRegRegF(irs, target, midReg, vsrc);
}

void MIRBinTranslator::transDivImmRegI(std::vector<MIR *> &irs,
                                       reg::Virtual *target, int imm,
                                       reg::Virtual *vsrc) {
  auto midReg = new reg::Virtual(ir::BasicType::I32);
  MIROpUtil::loadImmToReg(irs, midReg, imm);
  transDivRegRegI(irs, target, midReg, vsrc);
}

void MIRBinTranslator::transDivRegImmF(std::vector<MIR *> &irs,
                                       reg::Virtual *target, reg::Virtual *vsrc,
                                       float imm) {
  auto midReg = new reg::Virtual(ir::BasicType::FLOAT);
  MIROpUtil::loadImmToReg(irs, midReg, imm);
  transDivRegRegF(irs, target, vsrc, midReg);
}

void MIRBinTranslator::transDivRegImmI(std::vector<MIR *> &irs,
                                       reg::Virtual *target, reg::Virtual *vsrc,
                                       int imm) {
  if (imm == 1) {
    irs.push_back(new RR(RR::MV, target, vsrc));
    return;
  }
  if (imm == -1) {
    irs.push_back(new RR(RR::NEG, target, vsrc));
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
  auto midReg1 = new reg::Virtual(ir::BasicType::I32);
  auto midReg2 = new reg::Virtual(ir::BasicType::I32);
  auto midReg3 = new reg::Virtual(ir::BasicType::I32);
  auto midReg4 = new reg::Virtual(ir::BasicType::I32);
  MIROpUtil::loadImmToReg(irs, midReg1, magic);
  if (magic >= 0) {
    irs.push_back(new RRR(RRR::MUL, midReg2, vsrc, midReg1));
    irs.push_back(new RRI(RRI::SRLI, midReg2, midReg2, 32));
  } else {
    irs.push_back(new RRR(RRR::MUL, midReg2, vsrc, midReg1));
    irs.push_back(new RRI(RRI::SRLI, midReg2, midReg2, 32));
    irs.push_back(new RRR(RRR::ADD, midReg2, midReg2, vsrc));
  }
  if (shift != 0) {
    irs.push_back(new RRI(RRI::SRAIW, midReg3, midReg2, shift));
  } else {
    midReg3 = midReg2;
  }
  if (isPos) {
    irs.push_back(new RRI(RRI::SRLIW, midReg4, vsrc, 31));
    irs.push_back(new RRR(RRR::ADDW, target, midReg3, midReg4));
  } else {
    irs.push_back(new RRI(RRI::SRAIW, midReg4, vsrc, 31));
    irs.push_back(new RRR(RRR::SUBW, target, midReg3, midReg4));
  }
}

void MIRBinTranslator::transModImmReg(std::vector<MIR *> &irs,
                                      reg::Virtual *target, int imm,
                                      reg::Virtual *vsrc) {
  auto midReg = new reg::Virtual(ir::BasicType::I32);
  MIROpUtil::loadImmToReg(irs, midReg, imm);
  transModRegReg(irs, target, midReg, vsrc);
}

void MIRBinTranslator::transModRegImm(std::vector<MIR *> &irs,
                                      reg::Virtual *target, reg::Virtual *vsrc,
                                      int imm) {
  if (imm == 1) {
    irs.push_back(new RR(RR::MV, target, vsrc));
    return;
  }
  if (MIROpUtil::bitcount(imm) == 1) {
    auto midReg1 = new reg::Virtual(ir::BasicType::I32);
    if (imm == 2) {
      irs.push_back(new RRI(RRI::SRLIW, midReg1, vsrc, 31));
    } else {
      irs.push_back(new RRI(RRI::SRAIW, midReg1, vsrc, 31));
      irs.push_back(new RRI(RRI::SRLIW, midReg1, midReg1,
                            MIROpUtil::leadingZeros(imm) + 1));
    }
    irs.push_back(new RRR(RRR::ADD, midReg1, midReg1, vsrc));
    if (std::abs(imm) < 2048) {
      irs.push_back(new RRI(RRI::ANDI, midReg1, midReg1, -imm));
    } else {
      auto midReg2 = new reg::Virtual(ir::BasicType::I32);
      irs.push_back(new LoadImm(midReg1, -imm));
      irs.push_back(new RRR(RRR::AND, midReg1, midReg1, midReg2));
    }
    irs.push_back(new RRR(RRR::SUBW, target, vsrc, midReg1));
    return;
  }
  auto midReg1 = new reg::Virtual(ir::BasicType::I32);
  auto midReg2 = new reg::Virtual(ir::BasicType::I32);
  transDivRegImmI(irs, midReg1, vsrc, imm);
  transMulRegImmI(irs, midReg2, midReg1, imm);
  irs.push_back(new RRR(RRR::SUBW, target, vsrc, midReg2));
}

void MIRBinTranslator::transMulRegImmF(std::vector<MIR *> &irs,
                                       reg::Virtual *target, reg::Virtual *vsrc,
                                       float imm) {
  auto midReg = new reg::Virtual(ir::BasicType::FLOAT);
  MIROpUtil::loadImmToReg(irs, midReg, imm);
  transMulRegRegF(irs, target, vsrc, midReg);
}

void MIRBinTranslator::transMulRegImmI(std::vector<MIR *> &irs,
                                       reg::Virtual *target, reg::Virtual *vsrc,
                                       int imm) {
  if (imm == 0) {
    MIROpUtil::loadImmToReg(irs, target, 0);
    return;
  }
  if (imm == 1) {
    irs.push_back(new RR(RR::MV, target, vsrc));
    return;
  }
  if (imm == -1) {
    irs.push_back(new RR(RR::NEG, target, vsrc));
    return;
  }
  if (MIROpUtil::bitcount(imm) == 1) {
    irs.push_back(
        new RRI(RRI::SLLIW, target, vsrc, MIROpUtil::trailingZeros(imm)));
    return;
  }
  if (MIROpUtil::bitcount(imm) == 2 && imm % 2 == 1) {
    auto midReg = new reg::Virtual(ir::BasicType::I32);
    irs.push_back(
        new RRI(RRI::SLLIW, midReg, vsrc, 31 - MIROpUtil::leadingZeros(imm)));
    irs.push_back(new RRR(RRR::ADDW, target, midReg, vsrc));
    return;
  }
  if (MIROpUtil::trailingZeros(imm) == 0 &&
      MIROpUtil::leadingZeros(imm) + MIROpUtil::bitcount(imm) == 32) {
    auto midReg = new reg::Virtual(ir::BasicType::I32);
    irs.push_back(
        new RRI(RRI::SLLIW, midReg, vsrc, 32 - MIROpUtil::leadingZeros(imm)));
    irs.push_back(new RRR(RRR::SUBW, target, midReg, vsrc));
    return;
  }
  auto midReg = new reg::Virtual(ir::BasicType::I32);
  MIROpUtil::loadImmToReg(irs, midReg, imm);
  transMulRegRegI(irs, target, vsrc, midReg);
}

void MIRBinTranslator::transSubImmRegF(std::vector<MIR *> &irs,
                                       reg::Virtual *target, float imm,
                                       reg::Virtual *vsrc) {
  auto midReg = new reg::Virtual(ir::BasicType::FLOAT);
  MIROpUtil::loadImmToReg(irs, midReg, imm);
  transSubRegRegF(irs, target, midReg, vsrc);
}

void MIRBinTranslator::transSubImmRegI(std::vector<MIR *> &irs,
                                       reg::Virtual *target, int imm,
                                       reg::Virtual *vsrc) {
  auto midReg = new reg::Virtual(ir::BasicType::I32);
  MIROpUtil::loadImmToReg(irs, midReg, imm);
  transSubRegRegI(irs, target, midReg, vsrc);
}

void MIRBinTranslator::transSubRegImmF(std::vector<MIR *> &irs,
                                       reg::Virtual *target, reg::Virtual *vsrc,
                                       float imm) {
  auto midReg = new reg::Virtual(ir::BasicType::FLOAT);
  MIROpUtil::loadImmToReg(irs, midReg, imm);
  transSubRegRegF(irs, target, vsrc, midReg);
}

void MIRBinTranslator::transSubRegImmI(std::vector<MIR *> &irs,
                                       reg::Virtual *target, reg::Virtual *vsrc,
                                       int imm) {
  // TODO check here
  if (-imm >= -2048 && -imm < 2048) {
    irs.push_back(new RRI(RRI::ADDI, target, vsrc, -imm));
    return;
  }
  auto midReg = new reg::Virtual(ir::BasicType::I32);
  MIROpUtil::loadImmToReg(irs, midReg, imm);
  transSubRegRegI(irs, target, vsrc, midReg);
}

void MIRBinTranslator::transAddRegImmI(std::vector<MIR *> &irs,
                                       reg::Virtual *target, reg::Virtual *vsrc,
                                       int imm) {
  if (imm >= -2048 && imm < 2048) {
    irs.push_back(new RRI(RRI::ADDI, target, vsrc, imm));
    return;
  }
  auto midReg = new reg::Virtual(ir::BasicType::I32);
  MIROpUtil::loadImmToReg(irs, midReg, imm);
  transAddRegRegI(irs, target, vsrc, midReg);
}

void MIRBinTranslator::transBinImmReg(
    std::vector<MIR *> &irs,
    const std::unordered_map<ir::Instruction *, reg::Virtual *> &instRegMap,
    ir::BinaryInst *binOp, ir::ConstantNumber *value, reg::Virtual *reg) {
  auto target = instRegMap.at(binOp);
  switch (binOp->op) {
  case ir::BinaryInst::ADD:
    transAddRegImmI(irs, target, reg, value->intValue());
    break;
  case ir::BinaryInst::FADD:
    transAddRegImmF(irs, target, reg, value->floatValue());
    break;
  case ir::BinaryInst::SDIV:
    transDivImmRegI(irs, target, value->intValue(), reg);
    break;
  case ir::BinaryInst::FDIV:
    transDivImmRegF(irs, target, value->floatValue(), reg);
    break;
  case ir::BinaryInst::SREM:
    transModImmReg(irs, target, value->intValue(), reg);
    break;
  case ir::BinaryInst::MUL:
    transMulRegImmI(irs, target, reg, value->intValue());
    break;
  case ir::BinaryInst::FMUL:
    transMulRegImmF(irs, target, reg, value->floatValue());
    break;
  case ir::BinaryInst::SUB:
    transSubImmRegI(irs, target, value->intValue(), reg);
    break;
  case ir::BinaryInst::FSUB:
    transSubImmRegF(irs, target, value->floatValue(), reg);
    break;
  default:
    throw std::runtime_error(
        "unsupported binary operator in MIRBinTranslator::transBinImmReg");
  }
}

void MIRBinTranslator::transBinRegImm(
    std::vector<MIR *> &irs,
    const std::unordered_map<ir::Instruction *, reg::Virtual *> &instRegMap,
    ir::BinaryInst *binOp, reg::Virtual *reg, ir::ConstantNumber *value) {
  auto target = instRegMap.at(binOp);
  switch (binOp->op) {
  case ir::BinaryInst::ADD:
    transAddRegImmI(irs, target, reg, value->intValue());
    break;
  case ir::BinaryInst::FADD:
    transAddRegImmF(irs, target, reg, value->floatValue());
    break;
  case ir::BinaryInst::SDIV:
    transDivRegImmI(irs, target, reg, value->intValue());
    break;
  case ir::BinaryInst::FDIV:
    transDivRegImmF(irs, target, reg, value->floatValue());
    break;
  case ir::BinaryInst::SREM:
    transModRegImm(irs, target, reg, value->intValue());
    break;
  case ir::BinaryInst::MUL:
    transMulRegImmI(irs, target, reg, value->intValue());
    break;
  case ir::BinaryInst::FMUL:
    transMulRegImmF(irs, target, reg, value->floatValue());
    break;
  case ir::BinaryInst::SUB:
    transSubRegImmI(irs, target, reg, value->intValue());
    break;
  case ir::BinaryInst::FSUB:
    transSubRegImmF(irs, target, reg, value->floatValue());
    break;
  case ir::BinaryInst::XOR:
    auto const midReg = new reg::Virtual(ir::BasicType::I32);
    MIROpUtil::loadImmToReg(irs, midReg, value->intValue());
    irs.push_back(new RRR(RRR::XOR, target, reg, midReg));
    break;
  }
}

void MIRBinTranslator::transBinRegReg(
    std::vector<MIR *> &irs,
    const std::unordered_map<ir::Instruction *, reg::Virtual *> &instRegMap,
    ir::BinaryInst *binOp, reg::Virtual *reg1, reg::Virtual *reg2) {
  auto const target = instRegMap.at(binOp);
  switch (binOp->op) {
  case ir::BinaryInst::ADD:
    transAddRegRegI(irs, target, reg1, reg2);
    break;
  case ir::BinaryInst::FADD:
    transAddRegRegF(irs, target, reg1, reg2);
    break;
  case ir::BinaryInst::SDIV:
    transDivRegRegI(irs, target, reg1, reg2);
    break;
  case ir::BinaryInst::FDIV:
    transDivRegRegF(irs, target, reg1, reg2);
    break;
  case ir::BinaryInst::SREM:
    transModRegReg(irs, target, reg1, reg2);
    break;
  case ir::BinaryInst::MUL:
    transMulRegRegI(irs, target, reg1, reg2);
    break;
  case ir::BinaryInst::FMUL:
    transMulRegRegF(irs, target, reg1, reg2);
    break;
  case ir::BinaryInst::SUB:
    transSubRegRegI(irs, target, reg1, reg2);
    break;
  case ir::BinaryInst::FSUB:
    transSubRegRegF(irs, target, reg1, reg2);
    break;
  default:
    throw std::runtime_error(
        "unsupported binary operator in MIRBinTranslator::transBinRegReg");
  }
}

void MIRTranslator::branch(
    ir::BranchInst *branchInst, std::vector<MIR *> &mirs,
    std::unordered_map<ir::Instruction *, reg::Virtual *> const &instRegMap) {
  if (!branchInst->isConditional()) {
    auto const dest = branchInst->getOperand<ir::BasicBlock>(0);
    mirs.push_back(new Branch(Branch::NUL, nullptr, nullptr, dest));
    return;
  }
  auto const cond = branchInst->getOperand<ir::Value>(0);
  auto const trueBlock = branchInst->getOperand<ir::BasicBlock>(1);
  auto const falseBlock = branchInst->getOperand<ir::BasicBlock>(2);
  reg::Virtual *reg;
  if (auto const ir = dynamic_cast<ir::Instruction *>(cond)) {
    reg = instRegMap.at(ir);
  } else if (auto const value = dynamic_cast<ir::ConstantNumber *>(cond)) {
    auto const midReg = new reg::Virtual(value->getType());
    if (value->getType() == ir::BasicType::FLOAT) {
      MIROpUtil::loadImmToReg(mirs, midReg, value->floatValue());
    } else {
      MIROpUtil::loadImmToReg(mirs, midReg, value->intValue());
    }
    reg = midReg;
  } else {
    throw std::runtime_error(
        "unsupported branch condition in MIROpTranslator::transBranch");
  }
  mirs.push_back(new Branch(Branch::NE, reg, reg::Machine::ZERO, trueBlock));
  mirs.push_back(new Branch(Branch::NUL, nullptr, nullptr, falseBlock));
}

void MIRTranslator::binary(
    ir::BinaryInst *binOp, std::vector<MIR *> &mirs,
    std::unordered_map<ir::Instruction *, reg::Virtual *> const &instRegMap,
    std::unordered_map<ir::Argument *, std::pair<bool, int>> const
        &argOffsets) {
  auto const operand1 = binOp->getOperand<ir::Value>(0);
  auto const operand2 = binOp->getOperand<ir::Value>(1);
  reg::Virtual *reg1 = nullptr;
  if (auto const arg = dynamic_cast<ir::Argument *>(operand1)) {
    auto const midReg = new reg::Virtual(arg->getType() == ir::BasicType::FLOAT
                                             ? ir::BasicType::FLOAT
                                             : ir::BasicType::I32);
    mirs.push_back(new LoadFrom(argOffsets.at(arg).first
                                    ? LoadFrom::INNER_PARAM
                                    : LoadFrom::OUTER_PARAM,
                                midReg, argOffsets.at(arg).second));
    reg1 = midReg;
  } else if (auto const inst = dynamic_cast<ir::Instruction *>(operand1)) {
    reg1 = instRegMap.at(inst);
  }
  reg::Virtual *reg2 = nullptr;
  if (auto const arg = dynamic_cast<ir::Argument *>(operand2)) {
    auto const midReg = new reg::Virtual(arg->getType() == ir::BasicType::FLOAT
                                             ? ir::BasicType::FLOAT
                                             : ir::BasicType::I32);
    mirs.push_back(new LoadFrom(argOffsets.at(arg).first
                                    ? LoadFrom::INNER_PARAM
                                    : LoadFrom::OUTER_PARAM,
                                midReg, argOffsets.at(arg).second));
    reg2 = midReg;
  } else if (auto const inst = dynamic_cast<ir::Instruction *>(operand2)) {
    reg2 = instRegMap.at(inst);
  }
  if (reg1 && reg2) {
    MIRBinTranslator::transBinRegReg(mirs, instRegMap, binOp, reg1, reg2);
    return;
  }
  auto const value2 = dynamic_cast<ir::ConstantNumber *>(operand2);
  if (reg1 && value2) {
    MIRBinTranslator::transBinRegImm(mirs, instRegMap, binOp, reg1, value2);
    return;
  }
  auto const value1 = dynamic_cast<ir::ConstantNumber *>(operand1);
  if (reg2 && value1) {
    MIRBinTranslator::transBinImmReg(mirs, instRegMap, binOp, value1, reg2);
    return;
  }
  if (value1 && value2) {
    auto const midReg = new reg::Virtual(value1->getType());
    auto const type1 = value1->getType();
    if (type1 == ir::BasicType::I32) {
      MIROpUtil::loadImmToReg(mirs, midReg, value1->intValue());
    } else if (type1 == ir::BasicType::FLOAT) {
      MIROpUtil::loadImmToReg(mirs, midReg, value1->floatValue());
    } else {
      throw std::runtime_error(
          "unsupported constant number type in MIROpTranslator::transBin");
    }
    MIRBinTranslator::transBinRegImm(mirs, instRegMap, binOp, midReg, value2);
    return;
  }
  throw std::runtime_error(
      "unsupported binary operator in MIROpTranslator::transBin");
}

int MIRTranslator::call(
    ir::CallInst *callInst, std::vector<MIR *> &mirs,
    std::unordered_map<ir::Instruction *, reg::Virtual *> const &instRegMap,
    std::unordered_map<ir::Argument *, std::pair<bool, int>> const &argOffsets,
    std::unordered_map<ir::AllocaInst *, int> const &localOffsets) {
  auto func = callInst->getOperand<ir::Function>(0);
  std::vector<MIR *> saveCalleeIRs;
  int iSize = 0, fSize = 0;
  for (int i = 1; i < callInst->size(); i++) {
    auto const param = callInst->getOperand<ir::Value>(i);
    if (param->getType() == ir::BasicType::FLOAT) {
      if (fSize < reg::Machine::F_CALLER_REGS.size()) {
        if (auto const arg = dynamic_cast<ir::Argument *>(param)) {
          saveCalleeIRs.push_back(
              new LoadFrom(argOffsets.at(arg).first ? LoadFrom::INNER_PARAM
                                                    : LoadFrom::OUTER_PARAM,
                           reg::Machine::F_CALLER_REGS.at(fSize),
                           argOffsets.at(arg).second));
        } else if (auto const allocaInst =
                       dynamic_cast<ir::AllocaInst *>(param)) {
          saveCalleeIRs.push_back(new RR(RR::MV,
                                         reg::Machine::F_CALLER_REGS.at(fSize),
                                         instRegMap.at(allocaInst)));
        } else if (auto const inst = dynamic_cast<ir::Instruction *>(param)) {
          saveCalleeIRs.push_back(new RR(RR::MV,
                                         reg::Machine::F_CALLER_REGS.at(fSize),
                                         instRegMap.at(inst)));
        } else if (auto const value =
                       dynamic_cast<ir::ConstantNumber *>(param)) {
          MIROpUtil::loadImmToReg(saveCalleeIRs,
                                  reg::Machine::F_CALLER_REGS.at(fSize),
                                  value->floatValue());
        } else {
          throw std::runtime_error(
              "unsupported parameter type in MIROpTranslator::transCall");
        }
      } else {
        if (auto const arg = dynamic_cast<ir::Argument *>(param)) {
          auto midReg = new reg::Virtual(arg->getType());
          mirs.push_back(new LoadFrom(argOffsets.at(arg).first
                                          ? LoadFrom::INNER_PARAM
                                          : LoadFrom::OUTER_PARAM,
                                      midReg, argOffsets.at(arg).second));
          mirs.push_back(new StoreTo(
              StoreTo::CALL_PARAM, midReg,
              (std::max(
                   iSize - static_cast<int>(reg::Machine::I_CALLER_REGS.size()),
                   0) +
               std::max(
                   fSize - static_cast<int>(reg::Machine::I_CALLER_REGS.size()),
                   0)) *
                  8));
        } else if (auto const inst = dynamic_cast<ir::Instruction *>(param)) {
          mirs.push_back(new StoreTo(
              StoreTo::CALL_PARAM, instRegMap.at(inst),
              (std::max(
                   iSize - static_cast<int>(reg::Machine::I_CALLER_REGS.size()),
                   0) +
               std::max(
                   fSize - static_cast<int>(reg::Machine::I_CALLER_REGS.size()),
                   0)) *
                  8));
        } else if (auto const value =
                       dynamic_cast<ir::ConstantNumber *>(param)) {
          auto const midReg = new reg::Virtual(ir::BasicType::I32);
          MIROpUtil::loadImmToReg(mirs, midReg, value->floatValue());
          mirs.push_back(new StoreTo(
              StoreTo::CALL_PARAM, midReg,
              (std::max(
                   iSize - static_cast<int>(reg::Machine::I_CALLER_REGS.size()),
                   0) +
               std::max(
                   fSize - static_cast<int>(reg::Machine::I_CALLER_REGS.size()),
                   0)) *
                  8));
        } else {
          throw std::runtime_error(
              "unsupported parameter type in MIROpTranslator::transCall");
        }
      }
      fSize++;
    } else {
      if (iSize < reg::Machine::I_CALLER_REGS.size()) {
        if (auto const global = dynamic_cast<ir::GlobalVariable *>(param)) {
          saveCalleeIRs.push_back(
              new LLA(reg::Machine::I_CALLER_REGS.at(iSize), global));
        } else if (auto const allocaInst =
                       dynamic_cast<ir::AllocaInst *>(param)) {
          if (dynamic_cast<ir::PointerType *>(allocaInst->getType())) {
            saveCalleeIRs.push_back(
                new RegAddImm(reg::Machine::I_CALLER_REGS.at(iSize),
                              localOffsets.at(allocaInst)));
          } else {
            saveCalleeIRs.push_back(
                new RR(RR::MV, reg::Machine::I_CALLER_REGS.at(iSize),
                       instRegMap.at(allocaInst)));
          }
        } else if (auto const arg = dynamic_cast<ir::Argument *>(param)) {
          saveCalleeIRs.push_back(
              new LoadFrom(argOffsets.at(arg).first ? LoadFrom::INNER_PARAM
                                                    : LoadFrom::OUTER_PARAM,
                           reg::Machine::I_CALLER_REGS.at(iSize),
                           argOffsets.at(arg).second));
        } else if (auto const inst = dynamic_cast<ir::Instruction *>(param)) {
          saveCalleeIRs.push_back(new RR(RR::MV,
                                         reg::Machine::I_CALLER_REGS.at(iSize),
                                         instRegMap.at(inst)));
        } else if (auto const value =
                       dynamic_cast<ir::ConstantNumber *>(param)) {
          MIROpUtil::loadImmToReg(saveCalleeIRs,
                                  reg::Machine::I_CALLER_REGS.at(iSize),
                                  value->intValue());
        } else {
          throw std::runtime_error(
              "unsupported parameter type in MIROpTranslator::transCall");
        }
      } else {
        if (auto const allocaInst = dynamic_cast<ir::AllocaInst *>(param)) {
          auto const midReg = new reg::Virtual(ir::BasicType::I32);
          mirs.push_back(new RegAddImm(midReg, localOffsets.at(allocaInst)));
          mirs.push_back(new StoreTo(
              StoreTo::CALL_PARAM, midReg,
              (std::max(
                   iSize - static_cast<int>(reg::Machine::I_CALLER_REGS.size()),
                   0) +
               std::max(
                   fSize - static_cast<int>(reg::Machine::I_CALLER_REGS.size()),
                   0)) *
                  8));
        } else if (auto const arg = dynamic_cast<ir::Argument *>(param)) {
          auto const midReg = new reg::Virtual(arg->getType());
          mirs.push_back(new LoadFrom(argOffsets.at(arg).first
                                          ? LoadFrom::INNER_PARAM
                                          : LoadFrom::OUTER_PARAM,
                                      midReg, argOffsets.at(arg).second));
          mirs.push_back(new StoreTo(
              StoreTo::CALL_PARAM, midReg,
              (std::max(
                   iSize - static_cast<int>(reg::Machine::I_CALLER_REGS.size()),
                   0) +
               std::max(
                   fSize - static_cast<int>(reg::Machine::I_CALLER_REGS.size()),
                   0)) *
                  8));
        } else if (auto const inst = dynamic_cast<ir::Instruction *>(param)) {
          mirs.push_back(new StoreTo(
              StoreTo::CALL_PARAM, instRegMap.at(inst),
              (std::max(
                   iSize - static_cast<int>(reg::Machine::I_CALLER_REGS.size()),
                   0) +
               std::max(
                   fSize - static_cast<int>(reg::Machine::I_CALLER_REGS.size()),
                   0)) *
                  8));
        } else if (auto const value =
                       dynamic_cast<ir::ConstantNumber *>(param)) {
          auto const midReg = new reg::Virtual(ir::BasicType::I32);
          MIROpUtil::loadImmToReg(mirs, midReg, value->intValue());
          mirs.push_back(new StoreTo(
              StoreTo::CALL_PARAM, midReg,
              (std::max(
                   iSize - static_cast<int>(reg::Machine::I_CALLER_REGS.size()),
                   0) +
               std::max(
                   fSize - static_cast<int>(reg::Machine::I_CALLER_REGS.size()),
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
  mirs.insert(mirs.end(), saveCalleeIRs.begin(), saveCalleeIRs.end());
  mirs.push_back(new Call(func));
  if (callInst->getType() != ir::BasicType::VOID) {
    auto const target = instRegMap.at(callInst);
    auto const instType = callInst->getType();
    reg::Reg *reg;
    if (instType == ir::BasicType::I32) {
      reg = reg::Machine::A0;
    } else if (instType == ir::BasicType::FLOAT) {
      reg = reg::Machine::FA0;
    } else {
      throw std::runtime_error(
          "unsupported return type in MIROpTranslator::transCall");
    }
    mirs.push_back(new RR(RR::MV, target, reg));
  }
  return static_cast<int>(callInst->size()) - 1;
}

void MIRTranslator::getPtr(
    ir::GetPtrInst *getPtrInst, std::vector<MIR *> &mirs,
    std::unordered_map<ir::Instruction *, reg::Virtual *> const &instRegMap,
    std::unordered_map<ir::Argument *, std::pair<bool, int>> const &argOffsets,
    std::unordered_map<ir::AllocaInst *, int> const &localOffsets) {
  const auto ptr = getPtrInst->getOperand<ir::Value>(0);
  if (const auto glob = dynamic_cast<ir::GlobalVariable *>(ptr)) {
    const auto midReg1 = new reg::Virtual(ir::BasicType::I32);
    const auto midReg2 = new reg::Virtual(ir::BasicType::I32);
    const auto midReg3 = new reg::Virtual(ir::BasicType::I32);
    const auto midReg4 = new reg::Virtual(ir::BasicType::I32);
    mirs.push_back(new LLA(midReg1, glob));
    mirs.push_back(new LoadImm(
        midReg2,
        static_cast<int>(getPtrInst->getType()->baseType()->getSize()) / 8));
    auto *oprand = getPtrInst->getLastOperand<ir::Value>();
    if (const auto arg = dynamic_cast<ir::Argument *>(oprand)) {
      mirs.push_back(new LoadFrom(argOffsets.at(arg).first
                                      ? LoadFrom::INNER_PARAM
                                      : LoadFrom::OUTER_PARAM,
                                  midReg3, argOffsets.at(arg).second));
    } else if (const auto indexInst = dynamic_cast<ir::Instruction *>(oprand)) {
      mirs.push_back(new RR(RR::MV, midReg3, instRegMap.at(indexInst)));
    } else if (const auto value = dynamic_cast<ir::ConstantNumber *>(oprand)) {
      if (value->getType() == ir::BasicType::FLOAT) {
        const auto midReg = new reg::Virtual(ir::BasicType::I32);
        float f = value->floatValue();
        mirs.push_back(new LoadImm(midReg, *reinterpret_cast<int *>(&f)));
        mirs.push_back(new RR(RR::MV, midReg3, midReg));
      } else {
        mirs.push_back(new LoadImm(midReg3, value->intValue()));
      }
    } else {
      throw std::runtime_error(
          "unsupported getelementptr operand in MIRGenerator::funcToMir");
    }
    mirs.push_back(new RRR(RRR::MUL, midReg4, midReg2, midReg3));
    mirs.push_back(
        new RRR(RRR::ADD, instRegMap.at(getPtrInst), midReg1, midReg4));
    return;
  }
  if (const auto arg = dynamic_cast<ir::Argument *>(ptr)) {
    std::pair<bool, int> innerOffset = argOffsets.at(arg);
    const auto midReg1 = new reg::Virtual(ir::BasicType::I32);
    const auto midReg2 = new reg::Virtual(ir::BasicType::I32);
    const auto midReg3 = new reg::Virtual(ir::BasicType::I32);
    const auto midReg4 = new reg::Virtual(ir::BasicType::I32);
    mirs.push_back(new LoadFrom(innerOffset.first ? LoadFrom::INNER_PARAM
                                                  : LoadFrom::OUTER_PARAM,
                                midReg1, innerOffset.second));
    mirs.push_back(new LoadImm(
        midReg2,
        static_cast<int>(getPtrInst->getType()->baseType()->getSize()) / 8));
    auto *oprand = getPtrInst->getLastOperand<ir::Value>();
    if (const auto indexArg = dynamic_cast<ir::Argument *>(oprand)) {
      mirs.push_back(new LoadFrom(argOffsets.at(arg).first
                                      ? LoadFrom::INNER_PARAM
                                      : LoadFrom::OUTER_PARAM,
                                  midReg3, argOffsets.at(indexArg).second));
    } else if (const auto indexInst = dynamic_cast<ir::Instruction *>(oprand)) {
      mirs.push_back(new RR(RR::MV, midReg3, instRegMap.at(indexInst)));
    } else if (const auto value = dynamic_cast<ir::ConstantNumber *>(oprand)) {
      if (value->getType() == ir::BasicType::FLOAT) {
        const auto midReg = new reg::Virtual(ir::BasicType::I32);
        float f = value->floatValue();
        mirs.push_back(new LoadImm(midReg, *reinterpret_cast<int *>(&f)));
        mirs.push_back(new RR(RR::MV, midReg3, midReg));
      } else {
        mirs.push_back(new LoadImm(midReg3, value->intValue()));
      }
    } else {
      throw std::runtime_error(
          "unsupported getelementptr operand in MIRGenerator::funcToMir");
    }
    mirs.push_back(new RRR(RRR::MUL, midReg4, midReg2, midReg3));
    mirs.push_back(
        new RRR(RRR::ADD, instRegMap.at(getPtrInst), midReg1, midReg4));
    return;
  }
  if (const auto allocaInst = dynamic_cast<ir::AllocaInst *>(ptr)) {
    const auto midReg1 = new reg::Virtual(ir::BasicType::I32);
    const auto midReg2 = new reg::Virtual(ir::BasicType::I32);
    const auto midReg3 = new reg::Virtual(ir::BasicType::I32);
    const auto midReg4 = new reg::Virtual(ir::BasicType::I32);
    mirs.push_back(new RegAddImm(midReg1, localOffsets.at(allocaInst)));
    auto *oprand = getPtrInst->getLastOperand<ir::Value>();
    if (const auto arg = dynamic_cast<ir::Argument *>(oprand)) {
      mirs.push_back(new LoadFrom(argOffsets.at(arg).first
                                      ? LoadFrom::INNER_PARAM
                                      : LoadFrom::OUTER_PARAM,
                                  midReg2, argOffsets.at(arg).second));
    } else if (const auto indexInst = dynamic_cast<ir::Instruction *>(oprand)) {
      mirs.push_back(new RR(RR::MV, midReg2, instRegMap.at(indexInst)));
    } else if (const auto value = dynamic_cast<ir::ConstantNumber *>(oprand)) {
      if (value->getType() == ir::BasicType::FLOAT) {
        const auto midReg = new reg::Virtual(ir::BasicType::I32);
        float f = value->floatValue();
        mirs.push_back(new LoadImm(midReg, *reinterpret_cast<int *>(&f)));
        mirs.push_back(new RR(RR::MV, midReg2, midReg));
      } else {
        mirs.push_back(new LoadImm(midReg2, value->intValue()));
      }
    } else {
      throw std::runtime_error(
          "unsupported getelementptr operand in MIRGenerator::funcToMir");
    }
    if (getPtrInst->size() == 3) {
      mirs.push_back(new LoadImm(
          midReg3,
          static_cast<int>(ptr->getType()->baseType()->baseType()->getSize()) /
              8));
    } else {
      mirs.push_back(new LoadImm(
          midReg3,
          static_cast<int>(ptr->getType()->baseType()->getSize()) / 8));
    }
    mirs.push_back(new RRR(RRR::MUL, midReg4, midReg2, midReg3));
    mirs.push_back(
        new RRR(RRR::ADD, instRegMap.at(getPtrInst), midReg1, midReg4));
    return;
  }
  if (const auto ptrInst = dynamic_cast<ir::Instruction *>(ptr)) {
    const auto midReg1 = new reg::Virtual(ir::BasicType::I32);
    const auto midReg2 = new reg::Virtual(ir::BasicType::I32);
    const auto midReg3 = new reg::Virtual(ir::BasicType::I32);
    auto *oprand = getPtrInst->getLastOperand<ir::Value>();
    if (const auto arg = dynamic_cast<ir::Argument *>(oprand)) {
      mirs.push_back(new LoadFrom(argOffsets.at(arg).first
                                      ? LoadFrom::INNER_PARAM
                                      : LoadFrom::OUTER_PARAM,
                                  midReg1, argOffsets.at(arg).second));
    } else if (const auto indexInst = dynamic_cast<ir::Instruction *>(oprand)) {
      mirs.push_back(new RR(RR::MV, midReg1, instRegMap.at(indexInst)));
    } else if (const auto value = dynamic_cast<ir::ConstantNumber *>(oprand)) {
      if (value->getType() == ir::BasicType::FLOAT) {
        const auto midReg = new reg::Virtual(ir::BasicType::I32);
        float f = value->floatValue();
        mirs.push_back(new LoadImm(midReg, *reinterpret_cast<int *>(&f)));
        mirs.push_back(new RR(RR::MV, midReg1, midReg));
      } else {
        mirs.push_back(new LoadImm(midReg1, value->intValue()));
      }
    } else {
      throw std::runtime_error(
          "unsupported getelementptr operand in MIRGenerator::funcToMir");
    }
    if (getPtrInst->size() == 3) {
      mirs.push_back(new LoadImm(
          midReg2,
          static_cast<int>(ptr->getType()->baseType()->baseType()->getSize()) /
              8));
    } else {
      mirs.push_back(new LoadImm(
          midReg2,
          static_cast<int>(ptr->getType()->baseType()->getSize()) / 8));
    }
    mirs.push_back(new RRR(RRR::MUL, midReg3, midReg1, midReg2));
    mirs.push_back(new RRR(RRR::ADD, instRegMap.at(getPtrInst),
                           instRegMap.at(ptrInst), midReg3));
  }
}

void MIRTranslator::load(
    ir::LoadInst *loadInst, std::vector<MIR *> &mirs,
    std::unordered_map<ir::Instruction *, reg::Virtual *> const &instRegMap,
    std::unordered_map<ir::Argument *, std::pair<bool, int>> const &argOffsets,
    std::unordered_map<ir::AllocaInst *, int> const &localOffsets) {
  const auto ptr = loadInst->getOperand<ir::Value>(0);
  if (const auto glob = dynamic_cast<ir::GlobalVariable *>(ptr)) {
    const auto midReg = new reg::Virtual(ir::BasicType::I32);
    mirs.push_back(new LLA(midReg, glob));
    mirs.push_back(new Load(instRegMap.at(loadInst), midReg, 0, 4));
    return;
  }
  if (const auto arg = dynamic_cast<ir::Argument *>(ptr)) {
    std::pair<bool, int> innerOffset = argOffsets.at(arg);
    mirs.push_back(new LoadFrom(innerOffset.first ? LoadFrom::INNER_PARAM
                                                  : LoadFrom::OUTER_PARAM,
                                instRegMap.at(loadInst), innerOffset.second));
  }
  if (const auto allocaInst = dynamic_cast<ir::AllocaInst *>(ptr)) {
    const auto midReg = new reg::Virtual(ir::BasicType::I32);
    mirs.push_back(new RegAddImm(midReg, localOffsets.at(allocaInst)));
    mirs.push_back(new Load(
        instRegMap.at(loadInst), midReg, 0,
        static_cast<int>(allocaInst->getType()->baseType()->getSize()) / 8));
    return;
  }
  if (const auto ptrInst = dynamic_cast<ir::Instruction *>(ptr)) {
    mirs.push_back(new Load(
        instRegMap.at(loadInst), instRegMap.at(ptrInst), 0,
        static_cast<int>(ptrInst->getType()->baseType()->getSize()) / 8));
  }
}

void MIRTranslator::ret(
    ir::RetInst *retInst, std::vector<MIR *> &mirs,
    std::unordered_map<ir::Instruction *, reg::Virtual *> const &instRegMap,
    std::unordered_map<ir::Argument *, std::pair<bool, int>> const &argOffsets,
    Label *retLabel) {
  if (!retInst->empty()) {
    const auto retVal = retInst->getOperand<ir::Value>(0);
    if (const auto arg = dynamic_cast<ir::Argument *>(retVal)) {
      mirs.push_back(new LoadFrom(
          argOffsets.at(arg).first ? LoadFrom::INNER_PARAM
                                   : LoadFrom::OUTER_PARAM,
          retVal->getType() == ir::BasicType::I32 ? reg::Machine::A0
                                                  : reg::Machine::FA0,
          argOffsets.at(arg).second));
    } else if (const auto valueInst = dynamic_cast<ir::Instruction *>(retVal)) {
      mirs.push_back(new RR(RR::MV,
                            retVal->getType() == ir::BasicType::I32
                                ? reg::Machine::A0
                                : reg::Machine::FA0,
                            instRegMap.at(valueInst)));
    } else if (const auto value = dynamic_cast<ir::ConstantNumber *>(retVal)) {
      const auto type = value->getType();
      if (type == ir::BasicType::I32) {
        mirs.push_back(new LoadImm(reg::Machine::A0, value->intValue()));
      } else if (type == ir::BasicType::FLOAT) {
        const auto midReg = new reg::Virtual(ir::BasicType::I32);
        float f = value->floatValue();
        mirs.push_back(new LoadImm(midReg, *reinterpret_cast<int *>(&f)));
        mirs.push_back(new RR(RR::MV, reg::Machine::FA0, midReg));
      } else {
        throw std::runtime_error(
            "unsupported ret operand in MIRGenerator::funcToMir");
      }
    } else {
      throw std::runtime_error(
          "unsupported ret operand in MIRGenerator::funcToMir");
    }
  }
  mirs.push_back(
      new Branch(Branch::NUL, nullptr, nullptr, retLabel->getBlock()));
}

void MIRTranslator::store(
    ir::StoreInst *storeInst, std::vector<MIR *> &mirs,
    std::unordered_map<ir::Instruction *, reg::Virtual *> const &instRegMap,
    std::unordered_map<ir::Argument *, std::pair<bool, int>> const &argOffsets,
    std::unordered_map<ir::AllocaInst *, int> const &localOffsets) {
  const auto value = storeInst->getOperand<ir::Value>(0);
  const auto ptr = storeInst->getOperand<ir::Value>(1);
  if (const auto glob = dynamic_cast<ir::GlobalVariable *>(ptr)) {
    const auto midReg1 = new reg::Virtual(ir::BasicType::I32);
    const auto midReg2 = new reg::Virtual(ir::BasicType::I32);
    mirs.push_back(new LLA(midReg1, glob));
    if (const auto arg = dynamic_cast<ir::Argument *>(value)) {
      mirs.push_back(new LoadFrom(argOffsets.at(arg).first
                                      ? LoadFrom::INNER_PARAM
                                      : LoadFrom::OUTER_PARAM,
                                  midReg2, argOffsets.at(arg).second));
    } else if (const auto valueInst = dynamic_cast<ir::Instruction *>(value)) {
      mirs.push_back(new RR(RR::MV, midReg2, instRegMap.at(valueInst)));
    } else if (const auto v = dynamic_cast<ir::ConstantNumber *>(value)) {
      if (v->getType() == ir::BasicType::FLOAT) {
        const auto midReg = new reg::Virtual(ir::BasicType::I32);
        float f = v->floatValue();
        mirs.push_back(new LoadImm(midReg, *reinterpret_cast<int *>(&f)));
        mirs.push_back(new RR(RR::MV, midReg2, midReg));
      } else {
        mirs.push_back(new LoadImm(midReg2, v->intValue()));
      }
    } else {
      throw std::runtime_error(
          "unsupported store operand in MIRGenerator::funcToMir");
    }
    mirs.push_back(new Store(midReg2, midReg1, 0, 4));
    return;
  }
  if (const auto arg = dynamic_cast<ir::Argument *>(ptr)) {
    std::pair<bool, int> innerOffset = argOffsets.at(arg);
    const auto midReg1 = new reg::Virtual(ir::BasicType::I32);
    const auto midReg2 = new reg::Virtual(ir::BasicType::I32);
    mirs.push_back(new LoadFrom(innerOffset.first ? LoadFrom::INNER_PARAM
                                                  : LoadFrom::OUTER_PARAM,
                                midReg1, innerOffset.second));
    if (const auto argValue = dynamic_cast<ir::Argument *>(value)) {
      mirs.push_back(new LoadFrom(argOffsets.at(arg).first
                                      ? LoadFrom::INNER_PARAM
                                      : LoadFrom::OUTER_PARAM,
                                  midReg2, argOffsets.at(argValue).second));
    } else if (const auto valueInst = dynamic_cast<ir::Instruction *>(value)) {
      mirs.push_back(new RR(RR::MV, midReg2, instRegMap.at(valueInst)));
    } else if (const auto v = dynamic_cast<ir::ConstantNumber *>(value)) {
      if (v->getType() == ir::BasicType::FLOAT) {
        const auto midReg = new reg::Virtual(ir::BasicType::I32);
        float f = v->floatValue();
        mirs.push_back(new LoadImm(midReg, *reinterpret_cast<int *>(&f)));
        mirs.push_back(new RR(RR::MV, midReg2, midReg));
      } else {
        mirs.push_back(new LoadImm(midReg2, v->intValue()));
      }
    } else {
      throw std::runtime_error(
          "unsupported store operand in MIRGenerator::funcToMir");
    }
    mirs.push_back(new Store(midReg2, midReg1, 0, 4));
  }
  if (const auto allocaInst = dynamic_cast<ir::AllocaInst *>(ptr)) {
    const auto midReg1 = new reg::Virtual(ir::BasicType::I32);
    const auto midReg2 = new reg::Virtual(ir::BasicType::I32);
    if (const auto arg = dynamic_cast<ir::Argument *>(value)) {
      mirs.push_back(new LoadFrom(argOffsets.at(arg).first
                                      ? LoadFrom::INNER_PARAM
                                      : LoadFrom::OUTER_PARAM,
                                  midReg1, argOffsets.at(arg).second));
    } else if (const auto valueInst = dynamic_cast<ir::Instruction *>(value)) {
      mirs.push_back(new RR(RR::MV, midReg1, instRegMap.at(valueInst)));
    } else if (const auto v = dynamic_cast<ir::ConstantNumber *>(value)) {
      if (v->getType() == ir::BasicType::FLOAT) {
        const auto midReg = new reg::Virtual(ir::BasicType::I32);
        float f = v->floatValue();
        mirs.push_back(new LoadImm(midReg, *reinterpret_cast<int *>(&f)));
        mirs.push_back(new RR(RR::MV, midReg1, midReg));
      } else {
        mirs.push_back(new LoadImm(midReg1, v->intValue()));
      }
    } else {
      throw std::runtime_error(
          "unsupported store operand in MIRGenerator::funcToMir");
    }
    mirs.push_back(new RegAddImm(midReg2, localOffsets.at(allocaInst)));
    mirs.push_back(new Store(
        midReg1, midReg2, 0,
        static_cast<int>(allocaInst->getType()->baseType()->getSize()) / 8));
    return;
  }
  if (const auto ptrInst = dynamic_cast<ir::Instruction *>(ptr)) {
    const auto midReg = new reg::Virtual(ir::BasicType::I32);
    if (const auto arg = dynamic_cast<ir::Argument *>(value)) {
      mirs.push_back(new LoadFrom(argOffsets.at(arg).first
                                      ? LoadFrom::INNER_PARAM
                                      : LoadFrom::OUTER_PARAM,
                                  midReg, argOffsets.at(arg).second));
    } else if (const auto valueInst = dynamic_cast<ir::Instruction *>(value)) {
      mirs.push_back(new RR(RR::MV, midReg, instRegMap.at(valueInst)));
    } else if (const auto v = dynamic_cast<ir::ConstantNumber *>(value)) {
      if (v->getType() == ir::BasicType::FLOAT) {
        const auto midReg1 = new reg::Virtual(ir::BasicType::I32);
        float f = v->floatValue();
        mirs.push_back(new LoadImm(midReg1, *reinterpret_cast<int *>(&f)));
        mirs.push_back(new RR(RR::MV, midReg, midReg1));
      } else {
        mirs.push_back(new LoadImm(midReg, v->intValue()));
      }
    } else {
      throw std::runtime_error(
          "unsupported store operand in MIRGenerator::funcToMir");
    }
    mirs.push_back(new Store(
        midReg, instRegMap.at(ptrInst), 0,
        static_cast<int>(ptrInst->getType()->baseType()->getSize()) / 8));
  }
}

void MIRTranslator::bitCast(
    ir::BitCastInst *bitCastInst, std::vector<MIR *> &mirs,
    std::unordered_map<ir::Instruction *, reg::Virtual *> const &instRegMap,
    std::unordered_map<ir::AllocaInst *, int> const &localOffsets) {
  const auto operand = bitCastInst->getOperand<ir::Instruction>(0);
  reg::Virtual *srcReg = instRegMap.at(operand);
  if (const auto allocaInst = dynamic_cast<ir::AllocaInst *>(operand)) {
    srcReg = new reg::Virtual(ir::BasicType::I32);
    mirs.push_back(new RegAddImm(srcReg, localOffsets.at(allocaInst)));
  }
  mirs.push_back(new RR(RR::MV, instRegMap.at(bitCastInst), srcReg));
}

void MIRTranslator::iCmp(
    ir::ICmpInst *iCmpInst, std::vector<MIR *> &mirs,
    std::unordered_map<ir::Instruction *, reg::Virtual *> const &instRegMap,
    std::unordered_map<ir::Argument *, std::pair<bool, int>> const &argOffsets,
    std::unordered_map<ir::AllocaInst *, int> const &localOffsets) {
  reg::Virtual *target = instRegMap.at(iCmpInst);
  reg::Virtual *operand1;
  const auto op1 = iCmpInst->getOperand<ir::Value>(0);
  if (const auto arg = dynamic_cast<ir::Argument *>(op1)) {
    const auto midReg = new reg::Virtual(arg->getType() == ir::BasicType::FLOAT
                                             ? ir::BasicType::FLOAT
                                             : ir::BasicType::I32);
    mirs.push_back(new LoadFrom(argOffsets.at(arg).first
                                    ? LoadFrom::INNER_PARAM
                                    : LoadFrom::OUTER_PARAM,
                                midReg, argOffsets.at(arg).second));
    operand1 = midReg;
  } else if (const auto valueInst = dynamic_cast<ir::Instruction *>(op1)) {
    operand1 = instRegMap.at(valueInst);
  } else if (const auto value = dynamic_cast<ir::ConstantNumber *>(op1)) {
    const auto midReg = new reg::Virtual(value->getType());
    mirs.push_back(new LoadImm(midReg, value->intValue()));
    operand1 = midReg;
  } else {
    throw std::runtime_error(
        "unsupported icmp operand in MIRGenerator::funcToMir");
  }
  reg::Virtual *operand2;
  const auto op2 = iCmpInst->getOperand<ir::Value>(1);
  if (const auto arg = dynamic_cast<ir::Argument *>(op2)) {
    const auto midReg = new reg::Virtual(arg->getType() == ir::BasicType::FLOAT
                                             ? ir::BasicType::FLOAT
                                             : ir::BasicType::I32);
    mirs.push_back(new LoadFrom(argOffsets.at(arg).first
                                    ? LoadFrom::INNER_PARAM
                                    : LoadFrom::OUTER_PARAM,
                                midReg, argOffsets.at(arg).second));
    operand2 = midReg;
  } else if (const auto valueInst = dynamic_cast<ir::Instruction *>(op2)) {
    operand2 = instRegMap.at(valueInst);
  } else if (const auto value = dynamic_cast<ir::ConstantNumber *>(op2)) {
    const auto midReg = new reg::Virtual(value->getType());
    mirs.push_back(new LoadImm(midReg, value->intValue()));
    operand2 = midReg;
  } else {
    throw std::runtime_error(
        "unsupported icmp operand in MIRGenerator::funcToMir");
  }
  const auto midReg = new reg::Virtual(ir::BasicType::I32);
  switch (iCmpInst->getCond()) {
  case ir::CmpInst::EQ:
    mirs.push_back(new RRR(RRR::SUB, midReg, operand1, operand2));
    mirs.push_back(new RR(RR::SEQZ, target, midReg));
    break;
  case ir::CmpInst::NE:
    mirs.push_back(new RRR(RRR::SUB, midReg, operand1, operand2));
    mirs.push_back(new RR(RR::SNEZ, target, midReg));
    break;
  case ir::CmpInst::SGE:
    mirs.push_back(new RRR(RRR::SLT, midReg, operand1, operand2));
    mirs.push_back(new RRI(RRI::XORI, target, midReg, 1));
    break;
  case ir::CmpInst::SGT:
    mirs.push_back(new RRR(RRR::SGT, target, operand1, operand2));
    break;
  case ir::CmpInst::SLE:
    mirs.push_back(new RRR(RRR::SGT, midReg, operand1, operand2));
    mirs.push_back(new RRI(RRI::XORI, target, midReg, 1));
    break;
  case ir::CmpInst::SLT:
    if (const auto value = dynamic_cast<ir::ConstantNumber *>(op2)) {
      if (value->intValue() == 0) {
        mirs.pop_back();
        const auto mid = new reg::Virtual(value->getType());
        mirs.push_back(new RRI(RRI::SRAIW, mid, operand1, 31));
        mirs.push_back(new RRI(RRI::ANDI, target, mid, 1));
      } else if (value->intValue() >= -2048 && value->intValue() <= 2047) {
        mirs.pop_back();
        mirs.push_back(new RRI(RRI::SLTI, target, operand1, value->intValue()));
      } else {
        mirs.push_back(new RRR(RRR::SLT, target, operand1, operand2));
      }
    } else if (const auto zero = dynamic_cast<ir::ConstantZero *>(op2)) {
      mirs.pop_back();
      const auto mid = new reg::Virtual(zero->getType());
      mirs.push_back(new RRI(RRI::SRAIW, mid, operand1, 31));
      mirs.push_back(new RRI(RRI::ANDI, target, mid, 1));
    } else {
      mirs.push_back(new RRR(RRR::SLT, target, operand1, operand2));
    }

  default:
    // TODO need a throw?
    break;
  }
}

void MIRTranslator::fCmp(
    ir::FCmpInst *fCmpInst, std::vector<MIR *> &mirs,
    std::unordered_map<ir::Instruction *, reg::Virtual *> const &instRegMap,
    std::unordered_map<ir::Argument *, std::pair<bool, int>> const &argOffsets,
    std::unordered_map<ir::AllocaInst *, int> const &localOffsets) {
  reg::Virtual *target = instRegMap.at(fCmpInst);
  reg::Virtual *operand1;
  const auto op1 = fCmpInst->getOperand<ir::Value>(0);
  if (const auto arg = dynamic_cast<ir::Argument *>(op1)) {
    const auto midReg = new reg::Virtual(arg->getType() == ir::BasicType::FLOAT
                                             ? ir::BasicType::FLOAT
                                             : ir::BasicType::I32);
    mirs.push_back(new LoadFrom(argOffsets.at(arg).first
                                    ? LoadFrom::INNER_PARAM
                                    : LoadFrom::OUTER_PARAM,
                                midReg, argOffsets.at(arg).second));
    operand1 = midReg;
  } else if (const auto valueInst = dynamic_cast<ir::Instruction *>(op1)) {
    operand1 = instRegMap.at(valueInst);
  } else if (const auto value = dynamic_cast<ir::ConstantNumber *>(op1)) {
    const auto midReg1 = new reg::Virtual(ir::BasicType::I32);
    const auto midReg2 = new reg::Virtual(ir::BasicType::FLOAT);
    float f = value->floatValue();
    mirs.push_back(new LoadImm(midReg1, *reinterpret_cast<int *>(&f)));
    mirs.push_back(new RR(RR::MV, midReg2, midReg1));
    operand1 = midReg2;
  } else {
    throw std::runtime_error(
        "unsupported fcmp operand in MIRGenerator::funcToMir");
  }
  reg::Virtual *operand2;
  const auto op2 = fCmpInst->getOperand<ir::Value>(1);
  if (const auto arg = dynamic_cast<ir::Argument *>(op2)) {
    const auto midReg = new reg::Virtual(arg->getType() == ir::BasicType::FLOAT
                                             ? ir::BasicType::FLOAT
                                             : ir::BasicType::I32);
    mirs.push_back(new LoadFrom(argOffsets.at(arg).first
                                    ? LoadFrom::INNER_PARAM
                                    : LoadFrom::OUTER_PARAM,
                                midReg, argOffsets.at(arg).second));
    operand2 = midReg;
  } else if (const auto valueInst = dynamic_cast<ir::Instruction *>(op2)) {
    operand2 = instRegMap.at(valueInst);
  } else if (const auto value = dynamic_cast<ir::ConstantNumber *>(op2)) {
    const auto midReg1 = new reg::Virtual(ir::BasicType::I32);
    const auto midReg2 = new reg::Virtual(ir::BasicType::FLOAT);
    float f = value->floatValue();
    mirs.push_back(new LoadImm(midReg1, *reinterpret_cast<int *>(&f)));
    mirs.push_back(new RR(RR::MV, midReg2, midReg1));
    operand2 = midReg2;
  } else {
    throw std::runtime_error(
        "unsupported fcmp operand in MIRGenerator::funcToMir");
  }
  if (fCmpInst->getCond() == ir::CmpInst::UNE) {
    mirs.push_back(new RRR(RRR::EQ, target, operand1, operand2));
    mirs.push_back(new RRI(RRI::XORI, target, target, 1));
    return;
  }
  RRR::Op op;
  switch (fCmpInst->getCond()) {
  case ir::CmpInst::OEQ:
    op = RRR::EQ;
    break;
  case ir::CmpInst::OGE:
    op = RRR::GE;
    break;
  case ir::CmpInst::OGT:
    op = RRR::GT;
    break;
  case ir::CmpInst::OLE:
    op = RRR::LE;
    break;
  case ir::CmpInst::OLT:
    op = RRR::LT;
    break;
  default:
    throw std::runtime_error(
        "unsupported fcmp operand in MIRGenerator::funcToMir");
  }
  mirs.push_back(new RRR(op, target, operand1, operand2));
}

} // namespace mir
