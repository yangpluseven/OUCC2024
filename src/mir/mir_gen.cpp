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
  iCallerNum =
      std::min(iCallerNum, static_cast<int>(reg::Machine::I_CALLER_REGS.size()));
  fCallerNum =
      std::min(fCallerNum, static_cast<int>(reg::Machine::F_CALLER_REGS.size()));
  int iSize = 0, fSize = 0;
  for (const auto arg : args) {
    const auto bType = dynamic_cast<ir::BasicType *>(arg->getType());
    if (!bType || arg->getType() == ir::BasicType::I32) {
      if (iSize < reg::Machine::I_CALLER_REGS.size()) {
        argOffsets[arg] = {true, (iCallerNum + fCallerNum - iSize - 1) * 8};
      } else {
        argOffsets[arg] = {
            false,
            (std::max(iSize - static_cast<int>(reg::Machine::I_CALLER_REGS.size()),
                      0) +
             std::max(fSize - static_cast<int>(reg::Machine::F_CALLER_REGS.size()),
                      0)) *
                8};
      }
      iSize++;
    } else {
      if (fSize < reg::Machine::F_CALLER_REGS.size()) {
        argOffsets[arg] = {true, (fCallerNum - fSize - 1) * 8};
      } else {
        argOffsets[arg] = {
            false,
            (std::max(iSize - static_cast<int>(reg::Machine::I_CALLER_REGS.size()),
                      0) +
             std::max(fSize - static_cast<int>(reg::Machine::F_CALLER_REGS.size()),
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
  auto retLabelMIR = new Label(new ir::BasicBlock(func));
  std::unordered_map<ir::Instruction *, reg::Virtual *> instRegMap;
  for (const auto block : *func) {
    for (const auto inst : *block) {
      instRegMap[inst] = new reg::Virtual(inst->getType() == ir::BasicType::FLOAT
                                           ? ir::BasicType::FLOAT
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
      if (const auto binaryOp = dynamic_cast<ir::Binary *>(inst)) {
        MIROpTranslator::transBin(mirs, instRegMap, argOffsets,
                                  binaryOp);
        continue;
      }
      if (const auto branchInst = dynamic_cast<ir::BranchInst *>(inst)) {
        MIROpTranslator::transBranch(mirs, instRegMap, branchInst);
        continue;
      }
      if (const auto callInst = dynamic_cast<ir::CallInst *>(inst)) {
        int paramNum = MIROpTranslator::transCall(
            mirs, instRegMap, argOffsets, callInst, localOffsets);
        mFunc->maxFuncParamNum = std::max(mFunc->maxFuncParamNum, paramNum);
        continue;
      }
      if (const auto getElePtrInst =
              dynamic_cast<ir::GetElementPtrInst *>(inst)) {
        const auto ptr = getElePtrInst->getOperand<ir::Value>(0);
        if (const auto glob = dynamic_cast<ir::GlobalVariable *>(ptr)) {
          const auto midReg1 = new reg::Virtual(ir::BasicType::I32);
          const auto midReg2 = new reg::Virtual(ir::BasicType::I32);
          const auto midReg3 = new reg::Virtual(ir::BasicType::I32);
          const auto midReg4 = new reg::Virtual(ir::BasicType::I32);
          mirs.push_back(new LLA(midReg1, glob));
          mirs.push_back(new LoadImm(
              midReg2, static_cast<int>(
                           getElePtrInst->getType()->baseType()->getSize()) /
                           8));
          auto *oprand = getElePtrInst->getLastOperand<ir::Value>();
          if (const auto arg = dynamic_cast<ir::Argument *>(oprand)) {
            mirs.push_back(new LoadFrom(
                argOffsets.at(arg).first ? LoadFrom::INNER_PARAM
                                         : LoadFrom::OUTER_PARAM,
                midReg3, argOffsets.at(arg).second));
          } else if (const auto indexInst =
                         dynamic_cast<ir::Instruction *>(oprand)) {
            mirs.push_back(
                new RR(RR::MV, midReg3, instRegMap.at(indexInst)));
          } else if (const auto value =
                         dynamic_cast<ir::ConstantNumber *>(oprand)) {
            if (value->getType() == ir::BasicType::FLOAT) {
              const auto midReg = new reg::Virtual(ir::BasicType::I32);
              float f = value->floatValue();
              mirs.push_back(
                  new LoadImm(midReg, *reinterpret_cast<int *>(&f)));
              mirs.push_back(new RR(RR::MV, midReg3, midReg));
            } else {
              mirs.push_back(new LoadImm(midReg3, value->intValue()));
            }
          } else {
            throw std::runtime_error(
                "unsupported getelementptr operand in MIRGenerator::funcToMir");
          }
          mirs.push_back(
              new RRR(RRR::MUL, midReg4, midReg2, midReg3));
          mirs.push_back(new RRR(
              RRR::ADD, instRegMap.at(getElePtrInst), midReg1, midReg4));
          continue;
        }
        if (const auto arg = dynamic_cast<ir::Argument *>(ptr)) {
          std::pair<bool, int> innerOffset = argOffsets.at(arg);
          const auto midReg1 = new reg::Virtual(ir::BasicType::I32);
          const auto midReg2 = new reg::Virtual(ir::BasicType::I32);
          const auto midReg3 = new reg::Virtual(ir::BasicType::I32);
          const auto midReg4 = new reg::Virtual(ir::BasicType::I32);
          mirs.push_back(
              new LoadFrom(innerOffset.first ? LoadFrom::INNER_PARAM
                                                : LoadFrom::OUTER_PARAM,
                              midReg1, innerOffset.second));
          mirs.push_back(new LoadImm(
              midReg2, static_cast<int>(
                           getElePtrInst->getType()->baseType()->getSize()) /
                           8));
          auto *oprand = getElePtrInst->getLastOperand<ir::Value>();
          if (const auto indexArg = dynamic_cast<ir::Argument *>(oprand)) {
            mirs.push_back(new LoadFrom(
                argOffsets.at(arg).first ? LoadFrom::INNER_PARAM
                                         : LoadFrom::OUTER_PARAM,
                midReg3, argOffsets.at(indexArg).second));
          } else if (const auto indexInst =
                         dynamic_cast<ir::Instruction *>(oprand)) {
            mirs.push_back(
                new RR(RR::MV, midReg3, instRegMap.at(indexInst)));
          } else if (const auto value =
                         dynamic_cast<ir::ConstantNumber *>(oprand)) {
            if (value->getType() == ir::BasicType::FLOAT) {
              const auto midReg = new reg::Virtual(ir::BasicType::I32);
              float f = value->floatValue();
              mirs.push_back(
                  new LoadImm(midReg, *reinterpret_cast<int *>(&f)));
              mirs.push_back(new RR(RR::MV, midReg3, midReg));
            } else {
              mirs.push_back(new LoadImm(midReg3, value->intValue()));
            }
          } else {
            throw std::runtime_error(
                "unsupported getelementptr operand in MIRGenerator::funcToMir");
          }
          mirs.push_back(
              new RRR(RRR::MUL, midReg4, midReg2, midReg3));
          mirs.push_back(new RRR(
              RRR::ADD, instRegMap.at(getElePtrInst), midReg1, midReg4));
          // TODO no continue?
        }
        if (const auto allocaInst = dynamic_cast<ir::AllocaInst *>(ptr)) {
          const auto midReg1 = new reg::Virtual(ir::BasicType::I32);
          const auto midReg2 = new reg::Virtual(ir::BasicType::I32);
          const auto midReg3 = new reg::Virtual(ir::BasicType::I32);
          const auto midReg4 = new reg::Virtual(ir::BasicType::I32);
          mirs.push_back(
              new RegAddImm(midReg1, localOffsets.at(allocaInst)));
          auto *oprand = getElePtrInst->getLastOperand<ir::Value>();
          if (const auto arg = dynamic_cast<ir::Argument *>(oprand)) {
            mirs.push_back(new LoadFrom(
                argOffsets.at(arg).first ? LoadFrom::INNER_PARAM
                                         : LoadFrom::OUTER_PARAM,
                midReg2, argOffsets.at(arg).second));
          } else if (const auto indexInst =
                         dynamic_cast<ir::Instruction *>(oprand)) {
            mirs.push_back(
                new RR(RR::MV, midReg2, instRegMap.at(indexInst)));
          } else if (const auto value =
                         dynamic_cast<ir::ConstantNumber *>(oprand)) {
            if (value->getType() == ir::BasicType::FLOAT) {
              const auto midReg = new reg::Virtual(ir::BasicType::I32);
              float f = value->floatValue();
              mirs.push_back(
                  new LoadImm(midReg, *reinterpret_cast<int *>(&f)));
              mirs.push_back(new RR(RR::MV, midReg2, midReg));
            } else {
              mirs.push_back(new LoadImm(midReg2, value->intValue()));
            }
          } else {
            throw std::runtime_error(
                "unsupported getelementptr operand in MIRGenerator::funcToMir");
          }
          if (getElePtrInst->size() == 3) {
            mirs.push_back(new LoadImm(
                midReg3,
                static_cast<int>(
                    ptr->getType()->baseType()->baseType()->getSize()) /
                    8));
          } else {
            mirs.push_back(new LoadImm(
                midReg3,
                static_cast<int>(ptr->getType()->baseType()->getSize()) / 8));
          }
          mirs.push_back(
              new RRR(RRR::MUL, midReg4, midReg2, midReg3));
          mirs.push_back(new RRR(
              RRR::ADD, instRegMap.at(getElePtrInst), midReg1, midReg4));
          continue;
        }
        if (const auto ptrInst = dynamic_cast<ir::Instruction *>(ptr)) {
          const auto midReg1 = new reg::Virtual(ir::BasicType::I32);
          const auto midReg2 = new reg::Virtual(ir::BasicType::I32);
          const auto midReg3 = new reg::Virtual(ir::BasicType::I32);
          auto *oprand = getElePtrInst->getLastOperand<ir::Value>();
          if (const auto arg = dynamic_cast<ir::Argument *>(oprand)) {
            mirs.push_back(new LoadFrom(
                argOffsets.at(arg).first ? LoadFrom::INNER_PARAM
                                         : LoadFrom::OUTER_PARAM,
                midReg1, argOffsets.at(arg).second));
          } else if (const auto indexInst =
                         dynamic_cast<ir::Instruction *>(oprand)) {
            mirs.push_back(
                new RR(RR::MV, midReg1, instRegMap.at(indexInst)));
          } else if (const auto value =
                         dynamic_cast<ir::ConstantNumber *>(oprand)) {
            if (value->getType() == ir::BasicType::FLOAT) {
              const auto midReg = new reg::Virtual(ir::BasicType::I32);
              float f = value->floatValue();
              mirs.push_back(
                  new LoadImm(midReg, *reinterpret_cast<int *>(&f)));
              mirs.push_back(new RR(RR::MV, midReg1, midReg));
            } else {
              mirs.push_back(new LoadImm(midReg1, value->intValue()));
            }
          } else {
            throw std::runtime_error(
                "unsupported getelementptr operand in MIRGenerator::funcToMir");
          }
          if (getElePtrInst->size() == 3) {
            mirs.push_back(new LoadImm(
                midReg2,
                static_cast<int>(
                    ptr->getType()->baseType()->baseType()->getSize()) /
                    8));
          } else {
            mirs.push_back(new LoadImm(
                midReg2,
                static_cast<int>(ptr->getType()->baseType()->getSize()) / 8));
          }
          mirs.push_back(
              new RRR(RRR::MUL, midReg3, midReg1, midReg2));
          mirs.push_back(
              new RRR(RRR::ADD, instRegMap.at(getElePtrInst),
                         instRegMap.at(ptrInst), midReg3));
        }
        continue;
      }
      if (const auto loadInst = dynamic_cast<ir::LoadInst *>(inst)) {
        const auto ptr = loadInst->getOperand<ir::Value>(0);
        if (const auto glob = dynamic_cast<ir::GlobalVariable *>(ptr)) {
          const auto midReg = new reg::Virtual(ir::BasicType::I32);
          mirs.push_back(new LLA(midReg, glob));
          mirs.push_back(
              new Load(instRegMap.at(loadInst), midReg, 0, 4));
          continue;
        }
        if (const auto arg = dynamic_cast<ir::Argument *>(ptr)) {
          std::pair<bool, int> innerOffset = argOffsets.at(arg);
          mirs.push_back(
              new LoadFrom(innerOffset.first ? LoadFrom::INNER_PARAM
                                                : LoadFrom::OUTER_PARAM,
                              instRegMap.at(loadInst), innerOffset.second));
        }
        if (const auto allocaInst = dynamic_cast<ir::AllocaInst *>(ptr)) {
          const auto midReg = new reg::Virtual(ir::BasicType::I32);
          mirs.push_back(
              new RegAddImm(midReg, localOffsets.at(allocaInst)));
          mirs.push_back(new Load(
              instRegMap.at(loadInst), midReg, 0,
              static_cast<int>(allocaInst->getType()->baseType()->getSize()) /
                  8));
          continue;
        }
        if (const auto ptrInst = dynamic_cast<ir::Instruction *>(ptr)) {
          mirs.push_back(new Load(
              instRegMap.at(loadInst), instRegMap.at(ptrInst), 0,
              static_cast<int>(ptrInst->getType()->baseType()->getSize()) / 8));
        }
        continue;
      }
      if (const auto retInst = dynamic_cast<ir::RetInst *>(inst)) {
        if (!retInst->empty()) {
          const auto retVal = retInst->getOperand<ir::Value>(0);
          if (const auto arg = dynamic_cast<ir::Argument *>(retVal)) {
            mirs.push_back(new LoadFrom(
                argOffsets.at(arg).first ? LoadFrom::INNER_PARAM
                                         : LoadFrom::OUTER_PARAM,
                retVal->getType() == ir::BasicType::I32 ? reg::Machine::A0
                                                        : reg::Machine::FA0,
                argOffsets.at(arg).second));
          } else if (const auto valueInst =
                         dynamic_cast<ir::Instruction *>(retVal)) {
            mirs.push_back(new RR(
                RR::MV,
                retVal->getType() == ir::BasicType::I32 ? reg::Machine::A0
                                                        : reg::Machine::FA0,
                instRegMap.at(valueInst)));
          } else if (const auto value =
                         dynamic_cast<ir::ConstantNumber *>(retVal)) {
            const auto type = value->getType();
            if (type == ir::BasicType::I32) {
              mirs.push_back(
                  new LoadImm(reg::Machine::A0, value->intValue()));
            } else if (type == ir::BasicType::FLOAT) {
              const auto midReg = new reg::Virtual(ir::BasicType::I32);
              float f = value->floatValue();
              mirs.push_back(
                  new LoadImm(midReg, *reinterpret_cast<int *>(&f)));
              mirs.push_back(
                  new RR(RR::MV, reg::Machine::FA0, midReg));
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
            new Branch(Branch::NUL, nullptr, nullptr, retLabelMIR->getBlock()));
        continue;
      }
      if (const auto storeInst = dynamic_cast<ir::StoreInst *>(inst)) {
        const auto value = storeInst->getOperand<ir::Value>(0);
        const auto ptr = storeInst->getOperand<ir::Value>(1);
        if (const auto glob = dynamic_cast<ir::GlobalVariable *>(ptr)) {
          const auto midReg1 = new reg::Virtual(ir::BasicType::I32);
          const auto midReg2 = new reg::Virtual(ir::BasicType::I32);
          mirs.push_back(new LLA(midReg1, glob));
          if (const auto arg = dynamic_cast<ir::Argument *>(value)) {
            mirs.push_back(new LoadFrom(
                argOffsets.at(arg).first ? LoadFrom::INNER_PARAM
                                         : LoadFrom::OUTER_PARAM,
                midReg2, argOffsets.at(arg).second));
          } else if (const auto valueInst =
                         dynamic_cast<ir::Instruction *>(value)) {
            mirs.push_back(
                new RR(RR::MV, midReg2, instRegMap.at(valueInst)));
          } else if (const auto v = dynamic_cast<ir::ConstantNumber *>(value)) {
            if (v->getType() == ir::BasicType::FLOAT) {
              const auto midReg = new reg::Virtual(ir::BasicType::I32);
              float f = v->floatValue();
              mirs.push_back(
                  new LoadImm(midReg, *reinterpret_cast<int *>(&f)));
              mirs.push_back(new RR(RR::MV, midReg2, midReg));
            } else {
              mirs.push_back(new LoadImm(midReg2, v->intValue()));
            }
          } else {
            throw std::runtime_error(
                "unsupported store operand in MIRGenerator::funcToMir");
          }
          mirs.push_back(new Store(midReg2, midReg1, 0, 4));
          continue;
        }
        if (const auto arg = dynamic_cast<ir::Argument *>(ptr)) {
          std::pair<bool, int> innerOffset = argOffsets.at(arg);
          const auto midReg1 = new reg::Virtual(ir::BasicType::I32);
          const auto midReg2 = new reg::Virtual(ir::BasicType::I32);
          mirs.push_back(
              new LoadFrom(innerOffset.first ? LoadFrom::INNER_PARAM
                                                : LoadFrom::OUTER_PARAM,
                              midReg1, innerOffset.second));
          if (const auto argValue = dynamic_cast<ir::Argument *>(value)) {
            mirs.push_back(new LoadFrom(
                argOffsets.at(arg).first ? LoadFrom::INNER_PARAM
                                         : LoadFrom::OUTER_PARAM,
                midReg2, argOffsets.at(argValue).second));
          } else if (const auto valueInst =
                         dynamic_cast<ir::Instruction *>(value)) {
            mirs.push_back(
                new RR(RR::MV, midReg2, instRegMap.at(valueInst)));
          } else if (const auto v = dynamic_cast<ir::ConstantNumber *>(value)) {
            if (v->getType() == ir::BasicType::FLOAT) {
              const auto midReg = new reg::Virtual(ir::BasicType::I32);
              float f = v->floatValue();
              mirs.push_back(
                  new LoadImm(midReg, *reinterpret_cast<int *>(&f)));
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
            mirs.push_back(new LoadFrom(
                argOffsets.at(arg).first ? LoadFrom::INNER_PARAM
                                         : LoadFrom::OUTER_PARAM,
                midReg1, argOffsets.at(arg).second));
          } else if (const auto valueInst =
                         dynamic_cast<ir::Instruction *>(value)) {
            mirs.push_back(
                new RR(RR::MV, midReg1, instRegMap.at(valueInst)));
          } else if (const auto v = dynamic_cast<ir::ConstantNumber *>(value)) {
            if (v->getType() == ir::BasicType::FLOAT) {
              const auto midReg = new reg::Virtual(ir::BasicType::I32);
              float f = v->floatValue();
              mirs.push_back(
                  new LoadImm(midReg, *reinterpret_cast<int *>(&f)));
              mirs.push_back(new RR(RR::MV, midReg1, midReg));
            } else {
              mirs.push_back(new LoadImm(midReg1, v->intValue()));
            }
          } else {
            throw std::runtime_error(
                "unsupported store operand in MIRGenerator::funcToMir");
          }
          mirs.push_back(
              new RegAddImm(midReg2, localOffsets.at(allocaInst)));
          mirs.push_back(new Store(
              midReg1, midReg2, 0,
              static_cast<int>(allocaInst->getType()->baseType()->getSize()) /
                  8));
          continue;
        }
        if (const auto ptrInst = dynamic_cast<ir::Instruction *>(ptr)) {
          const auto midReg = new reg::Virtual(ir::BasicType::I32);
          if (const auto arg = dynamic_cast<ir::Argument *>(value)) {
            mirs.push_back(new LoadFrom(
                argOffsets.at(arg).first ? LoadFrom::INNER_PARAM
                                         : LoadFrom::OUTER_PARAM,
                midReg, argOffsets.at(arg).second));
          } else if (const auto valueInst =
                         dynamic_cast<ir::Instruction *>(value)) {
            mirs.push_back(
                new RR(RR::MV, midReg, instRegMap.at(valueInst)));
          } else if (const auto v = dynamic_cast<ir::ConstantNumber *>(value)) {
            if (v->getType() == ir::BasicType::FLOAT) {
              const auto midReg1 = new reg::Virtual(ir::BasicType::I32);
              float f = v->floatValue();
              mirs.push_back(
                  new LoadImm(midReg1, *reinterpret_cast<int *>(&f)));
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
        continue;
      }
      if (dynamic_cast<ir::AllocaInst *>(inst)) {
        continue;
      }
      if (const auto bitCastInst = dynamic_cast<ir::BitCastInst *>(inst)) {
        const auto operand = bitCastInst->getOperand<ir::Instruction>(0);
        reg::Virtual *srcReg = instRegMap.at(operand);
        if (const auto allocaInst = dynamic_cast<ir::AllocaInst *>(operand)) {
          srcReg = new reg::Virtual(ir::BasicType::I32);
          mirs.push_back(
              new RegAddImm(srcReg, localOffsets.at(allocaInst)));
        }
        mirs.push_back(
            new RR(RR::MV, instRegMap.at(bitCastInst), srcReg));
        continue;
      }
      if (const auto iCmpInst = dynamic_cast<ir::ICmpInst *>(inst)) {
        reg::Virtual *target = instRegMap.at(iCmpInst);
        reg::Virtual *operand1;
        const auto op1 = iCmpInst->getOperand<ir::Value>(0);
        if (const auto arg = dynamic_cast<ir::Argument *>(op1)) {
          const auto midReg = new reg::Virtual(
              arg->getType() == ir::BasicType::FLOAT ? ir::BasicType::FLOAT
                                                     : ir::BasicType::I32);
          mirs.push_back(new LoadFrom(
              argOffsets.at(arg).first ? LoadFrom::INNER_PARAM
                                       : LoadFrom::OUTER_PARAM,
              midReg, argOffsets.at(arg).second));
          operand1 = midReg;
        } else if (const auto valueInst =
                       dynamic_cast<ir::Instruction *>(op1)) {
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
          const auto midReg = new reg::Virtual(
              arg->getType() == ir::BasicType::FLOAT ? ir::BasicType::FLOAT
                                                     : ir::BasicType::I32);
          mirs.push_back(new LoadFrom(
              argOffsets.at(arg).first ? LoadFrom::INNER_PARAM
                                       : LoadFrom::OUTER_PARAM,
              midReg, argOffsets.at(arg).second));
          operand2 = midReg;
        } else if (const auto valueInst =
                       dynamic_cast<ir::Instruction *>(op2)) {
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
          mirs.push_back(
              new RRR(RRR::SUB, midReg, operand1, operand2));
          mirs.push_back(new RR(RR::SEQZ, target, midReg));
          break;
        case ir::CmpInst::NE:
          mirs.push_back(
              new RRR(RRR::SUB, midReg, operand1, operand2));
          mirs.push_back(new RR(RR::SNEZ, target, midReg));
          break;
        case ir::CmpInst::SGE:
          mirs.push_back(
              new RRR(RRR::SLT, midReg, operand1, operand2));
          mirs.push_back(
              new RRI(RRI::XORI, target, midReg, 1));
          break;
        case ir::CmpInst::SGT:
          mirs.push_back(
              new RRR(RRR::SGT, target, operand1, operand2));
          break;
        case ir::CmpInst::SLE:
          mirs.push_back(
              new RRR(RRR::SGT, midReg, operand1, operand2));
          mirs.push_back(
              new RRI(RRI::XORI, target, midReg, 1));
          break;
        case ir::CmpInst::SLT:
          if (const auto value = dynamic_cast<ir::ConstantNumber *>(op2)) {
            if (value->intValue() == 0) {
              mirs.pop_back();
              const auto mid = new reg::Virtual(value->getType());
              mirs.push_back(
                  new RRI(RRI::SRAIW, mid, operand1, 31));
              mirs.push_back(
                  new RRI(RRI::ANDI, target, mid, 1));
            } else if (value->intValue() >= -2048 &&
                       value->intValue() <= 2047) {
              mirs.pop_back();
              mirs.push_back(new RRI(
                  RRI::SLTI, target, operand1, value->intValue()));
            } else {
              mirs.push_back(
                  new RRR(RRR::SLT, target, operand1, operand2));
            }
          } else if (const auto zero = dynamic_cast<ir::ConstantZero *>(op2)) {
            mirs.pop_back();
            const auto mid = new reg::Virtual(zero->getType());
            mirs.push_back(
                new RRI(RRI::SRAIW, mid, operand1, 31));
            mirs.push_back(new RRI(RRI::ANDI, target, mid, 1));
          } else {
            mirs.push_back(
                new RRR(RRR::SLT, target, operand1, operand2));
          }

        default:
          // TODO need a throw?
          break;
        }
        continue;
      }
      if (const auto fCmpInst = dynamic_cast<ir::FCmpInst *>(inst)) {
        reg::Virtual *target = instRegMap.at(fCmpInst);
        reg::Virtual *operand1;
        const auto op1 = fCmpInst->getOperand<ir::Value>(0);
        if (const auto arg = dynamic_cast<ir::Argument *>(op1)) {
          const auto midReg = new reg::Virtual(
              arg->getType() == ir::BasicType::FLOAT ? ir::BasicType::FLOAT
                                                     : ir::BasicType::I32);
          mirs.push_back(new LoadFrom(
              argOffsets.at(arg).first ? LoadFrom::INNER_PARAM
                                       : LoadFrom::OUTER_PARAM,
              midReg, argOffsets.at(arg).second));
          operand1 = midReg;
        } else if (const auto valueInst =
                       dynamic_cast<ir::Instruction *>(op1)) {
          operand1 = instRegMap.at(valueInst);
        } else if (const auto value = dynamic_cast<ir::ConstantNumber *>(op1)) {
          const auto midReg1 = new reg::Virtual(ir::BasicType::I32);
          const auto midReg2 = new reg::Virtual(ir::BasicType::FLOAT);
          float f = value->floatValue();
          mirs.push_back(
              new LoadImm(midReg1, *reinterpret_cast<int *>(&f)));
          mirs.push_back(new RR(RR::MV, midReg2, midReg1));
          operand1 = midReg2;
        } else {
          throw std::runtime_error(
              "unsupported fcmp operand in MIRGenerator::funcToMir");
        }
        reg::Virtual *operand2;
        const auto op2 = fCmpInst->getOperand<ir::Value>(1);
        if (const auto arg = dynamic_cast<ir::Argument *>(op2)) {
          const auto midReg = new reg::Virtual(
              arg->getType() == ir::BasicType::FLOAT ? ir::BasicType::FLOAT
                                                     : ir::BasicType::I32);
          mirs.push_back(new LoadFrom(
              argOffsets.at(arg).first ? LoadFrom::INNER_PARAM
                                       : LoadFrom::OUTER_PARAM,
              midReg, argOffsets.at(arg).second));
          operand2 = midReg;
        } else if (const auto valueInst =
                       dynamic_cast<ir::Instruction *>(op2)) {
          operand2 = instRegMap.at(valueInst);
        } else if (const auto value = dynamic_cast<ir::ConstantNumber *>(op2)) {
          const auto midReg1 = new reg::Virtual(ir::BasicType::I32);
          const auto midReg2 = new reg::Virtual(ir::BasicType::FLOAT);
          float f = value->floatValue();
          mirs.push_back(
              new LoadImm(midReg1, *reinterpret_cast<int *>(&f)));
          mirs.push_back(new RR(RR::MV, midReg2, midReg1));
          operand2 = midReg2;
        } else {
          throw std::runtime_error(
              "unsupported fcmp operand in MIRGenerator::funcToMir");
        }
        if (fCmpInst->getCond() == ir::CmpInst::UNE) {
          mirs.push_back(
              new RRR(RRR::EQ, target, operand1, operand2));
          mirs.push_back(
              new RRI(RRI::XORI, target, target, 1));
          continue;
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
        continue;
      }
      if (const auto zExtInst = dynamic_cast<ir::ZExtInst *>(inst)) {
        reg::Virtual *operand;
        const auto op = zExtInst->getOperand<ir::Value>(0);
        if (const auto arg = dynamic_cast<ir::Argument *>(op)) {
          const auto midReg = new reg::Virtual(
              arg->getType() == ir::BasicType::FLOAT ? ir::BasicType::FLOAT
                                                     : ir::BasicType::I32);
          mirs.push_back(new LoadFrom(
              argOffsets.at(arg).first ? LoadFrom::INNER_PARAM
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
        mirs.push_back(
            new RR(RR::MV, instRegMap.at(zExtInst), operand));
        continue;
      }
      if (const auto sExtInst = dynamic_cast<ir::SExtInst *>(inst)) {
        reg::Virtual *operand;
        const auto op = sExtInst->getOperand<ir::Value>(0);
        if (const auto arg = dynamic_cast<ir::Argument *>(op)) {
          const auto midReg = new reg::Virtual(
              arg->getType() == ir::BasicType::FLOAT ? ir::BasicType::FLOAT
                                                     : ir::BasicType::I32);
          mirs.push_back(new LoadFrom(
              argOffsets.at(arg).first ? LoadFrom::INNER_PARAM
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
        mirs.push_back(
            new RR(RR::NEG, instRegMap.at(sExtInst), operand));
        continue;
      }
      if (const auto fpTosiInst = dynamic_cast<ir::FPToSIInst *>(inst)) {
        reg::Virtual *operand;
        const auto op = fpTosiInst->getOperand<ir::Value>(0);
        if (const auto arg = dynamic_cast<ir::Argument *>(op)) {
          const auto midReg = new reg::Virtual(
              arg->getType() == ir::BasicType::FLOAT ? ir::BasicType::FLOAT
                                                     : ir::BasicType::I32);
          mirs.push_back(new LoadFrom(
              argOffsets.at(arg).first ? LoadFrom::INNER_PARAM
                                       : LoadFrom::OUTER_PARAM,
              midReg, argOffsets.at(arg).second));
          operand = midReg;
        } else if (const auto valueInst = dynamic_cast<ir::Instruction *>(op)) {
          operand = instRegMap.at(valueInst);
        } else if (const auto value = dynamic_cast<ir::ConstantNumber *>(op)) {
          const auto midReg1 = new reg::Virtual(ir::BasicType::I32);
          const auto midReg2 = new reg::Virtual(ir::BasicType::FLOAT);
          float f = value->floatValue();
          mirs.push_back(
              new LoadImm(midReg1, *reinterpret_cast<int *>(&f)));
          mirs.push_back(new RR(RR::MV, midReg2, midReg1));
          operand = midReg2;
        } else {
          throw std::runtime_error(
              "unsupported fptosi operand in MIRGenerator::funcToMir");
        }
        mirs.push_back(
            new RR(RR::CVT, instRegMap.at(fpTosiInst), operand));
        continue;
      }
      if (const auto siTofpInst = dynamic_cast<ir::SIToFPInst *>(inst)) {
        reg::Virtual *operand;
        const auto op = siTofpInst->getOperand<ir::Value>(0);
        if (const auto arg = dynamic_cast<ir::Argument *>(op)) {
          const auto midReg = new reg::Virtual(
              arg->getType() == ir::BasicType::FLOAT ? ir::BasicType::FLOAT
                                                     : ir::BasicType::I32);
          mirs.push_back(new LoadFrom(
              argOffsets.at(arg).first ? LoadFrom::INNER_PARAM
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
        mirs.push_back(
            new RR(RR::CVT, instRegMap.at(siTofpInst), operand));
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
          mirs.push_back(new LoadFrom(
              argOffsets.at(arg).first ? LoadFrom::INNER_PARAM
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
  mirs.push_back(retLabelMIR);
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
