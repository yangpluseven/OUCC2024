#include "mir_gen.hpp"
#include "translate.hpp"

namespace mir {
std::unordered_map<ir::Argument *, std::pair<bool, int>>
Generator::calcArgOffsets(std::vector<ir::Argument *> &args) {
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
  iCallerNum = std::min(iCallerNum,
                        static_cast<int>(reg::Machine::I_CALLER_REGS.size()));
  fCallerNum = std::min(fCallerNum,
                        static_cast<int>(reg::Machine::F_CALLER_REGS.size()));
  int iSize = 0, fSize = 0;
  for (const auto arg : args) {
    const auto bType = dynamic_cast<ir::BasicType *>(arg->getType());
    if (!bType || arg->getType() == ir::BasicType::I32) {
      if (iSize < reg::Machine::I_CALLER_REGS.size()) {
        argOffsets[arg] = {true, (iCallerNum + fCallerNum - iSize - 1) * 8};
      } else {
        argOffsets[arg] = {
            false, (std::max(iSize - static_cast<int>(
                                         reg::Machine::I_CALLER_REGS.size()),
                             0) +
                    std::max(fSize - static_cast<int>(
                                         reg::Machine::F_CALLER_REGS.size()),
                             0)) *
                       8};
      }
      iSize++;
    } else {
      if (fSize < reg::Machine::F_CALLER_REGS.size()) {
        argOffsets[arg] = {true, (fCallerNum - fSize - 1) * 8};
      } else {
        argOffsets[arg] = {
            false, (std::max(iSize - static_cast<int>(
                                         reg::Machine::I_CALLER_REGS.size()),
                             0) +
                    std::max(fSize - static_cast<int>(
                                         reg::Machine::F_CALLER_REGS.size()),
                             0)) *
                       8};
      }
      fSize++;
    }
  }
  return std::move(argOffsets);
}

MachineFunction *Generator::funcToMIR(ir::Function *func) {
  std::unordered_map<ir::Argument *, std::pair<bool, int>> argOffsets =
      calcArgOffsets(func->getArgs());
  std::pair<int, std::unordered_map<ir::AllocaInst *, int>> locals =
      calcLocalOffsets(func->getFirst());
  std::pair<int, int> callerNums = getCallerNumbers(func);
  auto *mFunc = new MachineFunction(func, locals.first, callerNums.first,
                                    callerNums.second);
  auto &mirs = mFunc->getMIRs();
  auto retLable = new Label(new ir::BasicBlock(func));
  std::unordered_map<ir::Instruction *, reg::Virtual *> instRegMap;
  for (const auto block : *func) {
    for (const auto inst : *block) {
      instRegMap[inst] = new reg::Virtual(
          inst->getType() == ir::BasicType::FLOAT ? ir::BasicType::FLOAT
                                                  : ir::BasicType::I32);
    }
  }
  for (const auto block : *func) {
    for (int i = 0; i < block->size(); i++) {
      ir::Instruction *inst = block->get(i);
      if (const auto phi = dynamic_cast<ir::PHINode *>(inst)) {
        for (int j = 0; j < phi->size(); j++) {
          std::pair<ir::BasicBlock *, ir::Value *> blockValue =
              phi->getBlockValue(j);
          ir::BasicBlock *tmpBlock = blockValue.first;
          ir::Value *value = blockValue.second;
          tmpBlock->insert(static_cast<int>(tmpBlock->size()) - 1,
                           new ir::FakeMvInst(tmpBlock, phi, value));
        }
        block->remove(i);
        i--;
      }
    }
  }
  std::unordered_map<ir::AllocaInst *, int> localOffsets = locals.second;
  for (const auto block : *func) {
    if (!mirs.empty()) {
      const auto lastInst = mirs.back();
      const auto branch = dynamic_cast<Branch *>(lastInst);
      if (branch && branch->op == Branch::NUL && branch->block == block) {
        mirs.pop_back();
      }
    }
    mFunc->addMIR(new Label(block));
    for (const auto inst : *block) {
      if (const auto binaryOp = dynamic_cast<ir::BinaryInst *>(inst)) {
        MIRTranslator::binary(binaryOp, mirs, instRegMap, argOffsets);
        continue;
      }
      if (const auto branchInst = dynamic_cast<ir::BranchInst *>(inst)) {
        MIRTranslator::branch(branchInst, mirs, instRegMap);
        continue;
      }
      if (const auto callInst = dynamic_cast<ir::CallInst *>(inst)) {
        int paramNum = MIRTranslator::call(callInst, mirs, instRegMap,
                                           argOffsets, localOffsets);
        mFunc->maxFuncParamNum = std::max(mFunc->maxFuncParamNum, paramNum);
        continue;
      }
      if (const auto getPtrInst = dynamic_cast<ir::GetPtrInst *>(inst)) {
        MIRTranslator::getPtr(getPtrInst, mirs, instRegMap, argOffsets,
                              localOffsets);
        continue;
      }
      if (const auto loadInst = dynamic_cast<ir::LoadInst *>(inst)) {
        MIRTranslator::load(loadInst, mirs, instRegMap, argOffsets,
                            localOffsets);
        continue;
      }
      if (const auto retInst = dynamic_cast<ir::RetInst *>(inst)) {
        MIRTranslator::ret(retInst, mirs, instRegMap, argOffsets, retLable);
        continue;
      }
      if (const auto storeInst = dynamic_cast<ir::StoreInst *>(inst)) {
        MIRTranslator::store(storeInst, mirs, instRegMap, argOffsets,
                             localOffsets);
        continue;
      }
      if (dynamic_cast<ir::AllocaInst *>(inst)) {
        continue;
      }
      if (const auto bitCastInst = dynamic_cast<ir::BitCastInst *>(inst)) {
        MIRTranslator::bitCast(bitCastInst, mirs, instRegMap, localOffsets);
        continue;
      }
      if (const auto iCmpInst = dynamic_cast<ir::ICmpInst *>(inst)) {
        MIRTranslator::iCmp(iCmpInst, mirs, instRegMap, argOffsets,
                            localOffsets);
        continue;
      }
      if (const auto fCmpInst = dynamic_cast<ir::FCmpInst *>(inst)) {
        MIRTranslator::fCmp(fCmpInst, mirs, instRegMap, argOffsets,
                            localOffsets);
        continue;
      }
      if (const auto zExtInst = dynamic_cast<ir::ZExtInst *>(inst)) {
        reg::Virtual *operand;
        const auto op = zExtInst->getOperand<ir::Value>(0);
        if (const auto arg = dynamic_cast<ir::Argument *>(op)) {
          const auto midReg = new reg::Virtual(
              arg->getType() == ir::BasicType::FLOAT ? ir::BasicType::FLOAT
                                                     : ir::BasicType::I32);
          mirs.push_back(new LoadFrom(argOffsets.at(arg).first
                                          ? LoadFrom::INNER_PARAM
                                          : LoadFrom::OUTER_PARAM,
                                      midReg, argOffsets.at(arg).second));
          operand = midReg;
        } else if (const auto valueInst = dynamic_cast<ir::Instruction *>(op)) {
          operand = instRegMap.at(valueInst);
        } else if (const auto value = dynamic_cast<ir::ConstantNumber *>(op)) {
          const auto midReg = new reg::Virtual(value->getType());
          mirs.push_back(new LoadImm(midReg, value->intValue()));
          operand = midReg;
        } else {
          throw std::runtime_error(
              "unsupported zext operand in MIRGenerator::funcToMir");
        }
        mirs.push_back(new RR(RR::MV, instRegMap.at(zExtInst), operand));
        continue;
      }
      if (const auto sExtInst = dynamic_cast<ir::SExtInst *>(inst)) {
        reg::Virtual *operand;
        const auto op = sExtInst->getOperand<ir::Value>(0);
        if (const auto arg = dynamic_cast<ir::Argument *>(op)) {
          const auto midReg = new reg::Virtual(
              arg->getType() == ir::BasicType::FLOAT ? ir::BasicType::FLOAT
                                                     : ir::BasicType::I32);
          mirs.push_back(new LoadFrom(argOffsets.at(arg).first
                                          ? LoadFrom::INNER_PARAM
                                          : LoadFrom::OUTER_PARAM,
                                      midReg, argOffsets.at(arg).second));
          operand = midReg;
        } else if (const auto valueInst = dynamic_cast<ir::Instruction *>(op)) {
          operand = instRegMap.at(valueInst);
        } else if (const auto value = dynamic_cast<ir::ConstantNumber *>(op)) {
          const auto midReg = new reg::Virtual(value->getType());
          mirs.push_back(new LoadImm(midReg, value->intValue()));
          operand = midReg;
        } else {
          throw std::runtime_error(
              "unsupported sext operand in MIRGenerator::funcToMir");
        }
        mirs.push_back(new RR(RR::NEG, instRegMap.at(sExtInst), operand));
        continue;
      }
      if (const auto fpTosiInst = dynamic_cast<ir::FPToSIInst *>(inst)) {
        reg::Virtual *operand;
        const auto op = fpTosiInst->getOperand<ir::Value>(0);
        if (const auto arg = dynamic_cast<ir::Argument *>(op)) {
          const auto midReg = new reg::Virtual(
              arg->getType() == ir::BasicType::FLOAT ? ir::BasicType::FLOAT
                                                     : ir::BasicType::I32);
          mirs.push_back(new LoadFrom(argOffsets.at(arg).first
                                          ? LoadFrom::INNER_PARAM
                                          : LoadFrom::OUTER_PARAM,
                                      midReg, argOffsets.at(arg).second));
          operand = midReg;
        } else if (const auto valueInst = dynamic_cast<ir::Instruction *>(op)) {
          operand = instRegMap.at(valueInst);
        } else if (const auto value = dynamic_cast<ir::ConstantNumber *>(op)) {
          const auto midReg1 = new reg::Virtual(ir::BasicType::I32);
          const auto midReg2 = new reg::Virtual(ir::BasicType::FLOAT);
          float f = value->floatValue();
          mirs.push_back(new LoadImm(midReg1, *reinterpret_cast<int *>(&f)));
          mirs.push_back(new RR(RR::MV, midReg2, midReg1));
          operand = midReg2;
        } else {
          throw std::runtime_error(
              "unsupported fptosi operand in MIRGenerator::funcToMir");
        }
        mirs.push_back(new RR(RR::CVT, instRegMap.at(fpTosiInst), operand));
        continue;
      }
      if (const auto siTofpInst = dynamic_cast<ir::SIToFPInst *>(inst)) {
        reg::Virtual *operand;
        const auto op = siTofpInst->getOperand<ir::Value>(0);
        if (const auto arg = dynamic_cast<ir::Argument *>(op)) {
          const auto midReg = new reg::Virtual(
              arg->getType() == ir::BasicType::FLOAT ? ir::BasicType::FLOAT
                                                     : ir::BasicType::I32);
          mirs.push_back(new LoadFrom(argOffsets.at(arg).first
                                          ? LoadFrom::INNER_PARAM
                                          : LoadFrom::OUTER_PARAM,
                                      midReg, argOffsets.at(arg).second));
          operand = midReg;
        } else if (const auto valueInst = dynamic_cast<ir::Instruction *>(op)) {
          operand = instRegMap.at(valueInst);
        } else if (const auto value = dynamic_cast<ir::ConstantNumber *>(op)) {
          const auto midReg = new reg::Virtual(value->getType());
          mirs.push_back(new LoadImm(midReg, value->intValue()));
          operand = midReg;
        } else {
          throw std::runtime_error(
              "unsupported sitofp operand in MIRGenerator::funcToMir");
        }
        mirs.push_back(new RR(RR::CVT, instRegMap.at(siTofpInst), operand));
        continue;
      }
      if (const auto fakeMvInst = dynamic_cast<ir::FakeMvInst *>(inst)) {
        const auto phi = fakeMvInst->getOperand<ir::PHINode>(0);
        reg::Virtual *target = instRegMap.at(phi);
        const auto src = fakeMvInst->getOperand<ir::Value>(1);
        reg::Virtual *vsrc = nullptr;
        if (const auto arg = dynamic_cast<ir::Argument *>(src)) {
          const auto midReg = new reg::Virtual(
              arg->getType() == ir::BasicType::FLOAT ? ir::BasicType::FLOAT
                                                     : ir::BasicType::I32);
          mirs.push_back(new LoadFrom(argOffsets.at(arg).first
                                          ? LoadFrom::INNER_PARAM
                                          : LoadFrom::OUTER_PARAM,
                                      midReg, argOffsets.at(arg).second));
          vsrc = midReg;
        } else if (const auto tmpInst = dynamic_cast<ir::Instruction *>(src)) {
          vsrc = instRegMap.at(tmpInst);
        } else if (const auto value = dynamic_cast<ir::ConstantNumber *>(src)) {
          const auto midReg = new reg::Virtual(value->getType());
          const auto type = midReg->getType();
          if (type == ir::BasicType::I32) {
            mirs.push_back(new LoadImm(midReg, value->intValue()));
          } else if (type == ir::BasicType::FLOAT) {
            const auto tmpMidReg = new reg::Virtual(ir::BasicType::I32);
            float f = value->floatValue();
            // TODO reinterpret_cast
            mirs.push_back(
                new LoadImm(tmpMidReg, *reinterpret_cast<int *>(&f)));
            mirs.push_back(new RR(RR::MV, midReg, tmpMidReg));
          } else {
            throw std::runtime_error(
                "Unsupported type in MIRGenerator::funcToMir");
          }
          vsrc = midReg;
        }
        mirs.push_back(new RR(RR::MV, target, vsrc));
        continue;
      }
      throw std::runtime_error(
          "Unsupported instruction in MIRGenerator::funcToMir");
    }
  }
  mirs.push_back(retLable);
  return mFunc;
}

std::pair<int, std::unordered_map<ir::AllocaInst *, int>>
Generator::calcLocalOffsets(ir::BasicBlock *block) {
  int localSize = 0;
  std::unordered_map<ir::AllocaInst *, int> localOffsets;
  for (const auto ir : *block) {
    auto const allocaInst = dynamic_cast<ir::AllocaInst *>(ir);
    if (!allocaInst) {
      break;
    }
    int size =
        static_cast<int>(allocaInst->getType()->baseType()->getSize() / 8);
    localOffsets[allocaInst] = localSize;
    localSize += size;
  }
  return {localSize, std::move(localOffsets)};
}

} // namespace mir
