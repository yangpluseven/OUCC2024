#include "mir/mir_gen.hpp"

std::unordered_map<ir::Argument *, std::pair<bool, int>>
mir::MIRGenerator::calcArgOffsets(std::vector<ir::Argument *> &args) {
  std::unordered_map<ir::Argument *, std::pair<bool, int>> argOffsets;
  int iCallerNum = 0, fCallerNum = 0;
  for (const auto arg : args) {
    if (dynamic_cast<ir::BasicType *>(arg->getType()) &&
        arg->getType() == ir::BasicType::FLOAT) {
      fCallerNum++;
    } else {
      iCallerNum++;
    }
  }
  iCallerNum =
      std::min(iCallerNum, static_cast<int>(reg::MReg::I_CALLER_REGS.size()));
  fCallerNum =
      std::min(fCallerNum, static_cast<int>(reg::MReg::F_CALLER_REGS.size()));
  int iSize = 0, fSize = 0;
  for (const auto arg : args) {
    const auto bType = dynamic_cast<ir::BasicType *>(arg->getType());
    if (!bType || arg->getType() == ir::BasicType::I32) {
      if (iSize < reg::MReg::I_CALLER_REGS.size()) {
        argOffsets[arg] = {true, (iCallerNum + fCallerNum - iSize - 1) * 8};
      } else {
        argOffsets[arg] = {
            false,
            (std::max(iSize - static_cast<int>(reg::MReg::I_CALLER_REGS.size()),
                      0) +
             std::max(fSize - static_cast<int>(reg::MReg::F_CALLER_REGS.size()),
                      0)) *
                8};
      }
      iSize++;
    } else {
      if (fSize < reg::MReg::F_CALLER_REGS.size()) {
        argOffsets[arg] = {true, (fCallerNum - fSize - 1) * 8};
      } else {
        argOffsets[arg] = {
            false,
            (std::max(iSize - static_cast<int>(reg::MReg::I_CALLER_REGS.size()),
                      0) +
             std::max(fSize - static_cast<int>(reg::MReg::F_CALLER_REGS.size()),
                      0)) *
                8};
      }
      fSize++;
    }
  }
  return std::move(argOffsets);
}

mir::MachineFunction *mir::MIRGenerator::funcToMir(ir::Function *func) {
  // TODO
}

std::pair<int, std::unordered_map<ir::AllocaInst *, int>>
mir::MIRGenerator::calcLocalOffsets(ir::BasicBlock *block) {
  int localSize = 0;
  std::unordered_map<ir::AllocaInst *, int> localOffsets;
  for (const auto ir : block) {
    auto const allocaInst = dynamic_cast<ir::AllocaInst *>(ir);
    if (!allocaInst) {
      break;
    }
    int size =
        static_cast<int>(allocaInst->getType()->baseType()->getSize() / 8);
    localOffsets[allocaInst] = localSize;
    localSize += size;
  }
  return {localSize, localOffsets};
}
