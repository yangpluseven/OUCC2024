#include "reg/reg_alloc.hpp"
#include <algorithm>

namespace reg {
std::vector<Block *> FuncRegAlloc::calcBlocks() const {
  auto const &irs = _func->getIRs();
  std::unordered_map<ir::BasicBlock *, int> lableIdMap;
  for (int i = 0; i < irs.size(); i++) {
    if (auto const labelMIR = dynamic_cast<mir::LabelMIR *>(irs.at(i))) {
      lableIdMap[labelMIR->getBlock()] = i;
    }
  }
  std::unordered_set<int> begins;
  begins.insert(0);
  std::unordered_map<int, int> jumpIdMap;
  std::unordered_map<int, bool> isBranchMap;
  for (int i = 0; i < irs.size(); i++) {
    if (auto const bMIR = dynamic_cast<mir::BMIR *>(irs.at(i))) {
      begins.insert(i + 1);
      jumpIdMap[i] = lableIdMap.at(bMIR->block);
      isBranchMap[i] = bMIR->hasCond();
      continue;
    }
    if (dynamic_cast<mir::LabelMIR *>(irs.at(i))) {
      begins.insert(i);
    }
  }
  begins.insert(static_cast<int>(irs.size()));
  std::vector<int> sortedBegins(begins.begin(), begins.end());
  std::sort(sortedBegins.begin(), sortedBegins.end());
  std::vector<Block *> blocks;
  std::unordered_map<int, Block *> blockBeginMap;
  for (int i = 0; i < static_cast<int>(sortedBegins.size()) - 1; i++) {
    int begin = sortedBegins.at(i);
    int end = sortedBegins.at(i + 1);
    auto block = new Block(begin, end);
    blocks.push_back(block);
    blockBeginMap[begin] = block;
  }
  for (auto block : blocks) {
    int end = block->getEnd();
    if (isBranchMap.find(end - 1) != isBranchMap.end()) {
      block->addNext(blockBeginMap.at(jumpIdMap.at(end - 1)));
      bool isBranch = isBranchMap.at(end - 1);
      if (isBranch) {
        block->addNext(blockBeginMap.at(end));
      }
      continue;
    }
    Block *next = nullptr;
    if (blockBeginMap.find(end) != blockBeginMap.end()) {
      next = blockBeginMap.at(end);
    }
    if (next != nullptr) {
      block->addNext(next);
    }
  }
  return std::move(blocks);
}

std::unordered_map<Reg *, std::unordered_set<int>>
FuncRegAlloc::calcLifespans() {
  auto const &irs = _func->getIRs();
  std::vector<Block *> blocks = calcBlocks();
  calcUseDef(blocks);
  calcUseDef(blocks);
  std::unordered_map<Reg *, std::unordered_set<int>> lifespans;
  for (auto block : blocks) {
    auto &regs = block->getOut();
    for (int i = block->getEnd() - 1; i >= block->getBegin(); i--) {
      for (auto reg : regs) {
        // TODO make sure this will work
        auto &lifespan = lifespans.try_emplace(reg).first->second;
        lifespan.insert(i);
      }
      mir::MIR *mir = irs.at(i);
      for (auto reg : mir->getWrite()) {
        auto &lifespan = lifespans.try_emplace(reg).first->second;
        lifespan.insert(i);
        regs.erase(reg);
      }
      for (auto reg : mir->getRead()) {
        auto &lifespan = lifespans.try_emplace(reg).first->second;
        lifespan.insert(i);
        regs.insert(reg);
      }
    }
  }
  return std::move(lifespans);
}

void FuncRegAlloc::popFrame() {
  std::vector<MReg *> toSaveRegs;
  if (_callAddrSize != 0) {
    toSaveRegs.push_back(MReg::RA);
  }
  toSaveRegs.insert(toSaveRegs.end(), _iCallerRegs.begin(), _iCallerRegs.end());
  toSaveRegs.insert(toSaveRegs.end(), _fCallerRegs.begin(), _fCallerRegs.end());
  toSaveRegs.insert(toSaveRegs.end(), _iCalleeRegs.begin(), _iCalleeRegs.end());
  toSaveRegs.insert(toSaveRegs.end(), _fCalleeRegs.begin(), _fCalleeRegs.end());
  int totalSize = static_cast<int>(toSaveRegs.size()) * 8 + _funcParamSize +
                  _alignSize + _spillSize + _localSize;
  if (totalSize > 0) {
    if (totalSize < 2048) {
      _func->addMIR(
          new mir::RriMIR(mir::RriMIR::ADDI, MReg::SP, MReg::SP, totalSize));
    } else {
      _func->addMIR(new mir::LiMIR(MReg::T0, totalSize));
      _func->addMIR(
          new mir::RrrMIR(mir::RrrMIR::ADD, MReg::SP, MReg::SP, MReg::T0));
    }
  }
  for (int i = 0; i < toSaveRegs.size(); i++) {
    auto toSaveReg = toSaveRegs[i];
    if (std::find(_iCallerRegs.begin(), _iCallerRegs.end(), toSaveReg) !=
            _iCallerRegs.end() ||
        std::find(_fCallerRegs.begin(), _fCallerRegs.end(), toSaveReg) !=
            _fCallerRegs.end()) {
      continue;
    }
    _func->addMIR(
        new mir::LoadMIR(toSaveReg, MReg::SP, -8 * (i + 1),
                         toSaveReg->getType() == ir::BasicType::I32 ? 8 : 4));
  }
}

void FuncRegAlloc::pushFrame() {
  auto &irs = _func->getIRs();
  std::vector<mir::MIR *> headIRs;
  std::vector<MReg *> toSaveRegs;
  if (_callAddrSize != 0) {
    toSaveRegs.push_back(MReg::RA);
  }
  toSaveRegs.insert(toSaveRegs.end(), _iCallerRegs.begin(), _iCallerRegs.end());
  toSaveRegs.insert(toSaveRegs.end(), _fCallerRegs.begin(), _fCallerRegs.end());
  toSaveRegs.insert(toSaveRegs.end(), _iCalleeRegs.begin(), _iCalleeRegs.end());
  toSaveRegs.insert(toSaveRegs.end(), _fCalleeRegs.begin(), _fCalleeRegs.end());
  for (int i = 0; i < toSaveRegs.size(); i++) {
    MReg *toSaveReg = toSaveRegs[i];
    headIRs.push_back(
        new mir::StoreMIR(toSaveReg, MReg::SP, -8 * (i + 1),
                          toSaveReg->getType() == ir::BasicType::I32 ? 8 : 4));
  }
  int totalSize = static_cast<int>(toSaveRegs.size()) * 8 + _funcParamSize +
                  _alignSize + _spillSize + _localSize;
  if (totalSize > 0 && totalSize <= 255) {
    headIRs.push_back(
        new mir::RriMIR(mir::RriMIR::ADDI, MReg::SP, MReg::SP, -totalSize));
  } else if (totalSize > 255) {
    headIRs.push_back(new mir::LiMIR(MReg::T0, totalSize));
    headIRs.push_back(
        new mir::RrrMIR(mir::RrrMIR::SUB, MReg::SP, MReg::SP, MReg::T0));
  }
  irs.insert(irs.begin(), headIRs.begin(), headIRs.end());
}

void FuncRegAlloc::solveSpill() {
  _spillSize = 0;
  bool toContinueOuter;
  do {
    toContinueOuter = false;
    std::unordered_map<Reg *, std::unordered_set<Reg *>> conflictMap =
        calcConflictMap();
    std::unordered_set<VReg *> allocatedVRegs;
    std::unordered_map<VReg *, MReg *> vRegToMRegMap;
    std::unordered_map<VReg *, int> spilledRegs;
    bool toContinueInner;
    do {
      toContinueInner = false;
      for (const auto &p : conflictMap) {
        if (auto const vreg = dynamic_cast<VReg *>(p.first)) {
          allocatedVRegs.insert(vreg);
          bool toSpill = true;
          const std::vector<MReg *> &regs =
              vreg->getType() == ir::BasicType::FLOAT ? MReg::F_REGS
                                                      : MReg::I_REGS;
          std::unordered_set<MReg *> usedRegs;
          for (auto const reg : conflictMap.at(vreg)) {
            if (auto const vreg1 = dynamic_cast<VReg *>(reg)) {
              MReg *mreg = vRegToMRegMap[vreg1];
              if (mreg) {
                usedRegs.insert(mreg);
              }
              continue;
            }
            if (auto const mreg = dynamic_cast<MReg *>(reg)) {
              usedRegs.insert(mreg);
              continue;
            }
            throw std::runtime_error(
                "unexpected reg type in FuncRegAlloc::solveSpill");
          }
          for (const auto mreg : regs) {
            if (usedRegs.find(mreg) != usedRegs.end()) {
              continue;
            }
            vRegToMRegMap[vreg] = mreg;
            toSpill = false;
            break;
          }
          if (toSpill) {
            VReg *toSpillReg = nullptr;
            int maxVal = 0;
            for (const auto reg : allocatedVRegs) {
              if (conflictMap.at(reg).size() > maxVal) {
                toSpillReg = reg;
                maxVal = static_cast<int>(conflictMap.at(reg).size());
              }
            }
            spilledRegs[toSpillReg] = _spillSize;
            _spillSize += 8;
            allocatedVRegs.erase(toSpillReg);
            conflictMap.erase(toSpillReg);
            // TODO will this work?
            for (auto &entry : conflictMap) {
              entry.second.erase(toSpillReg);
            }
            toContinueInner = true;
            toContinueOuter = true;
            break;
          }
        }
      }
    } while (toContinueInner);
    for (const auto &toSpill : spilledRegs) {
      VReg *reg = toSpill.first;
      int offset = toSpill.second;
      std::vector<mir::MIR *> newIRs;
      for (const auto &ir : _func->getIRs()) {
        auto vec = ir->getRegs();
        if (std::find(vec.begin(), vec.end(), reg) != vec.end()) {
          auto sp = ir->spill(reg, offset);
          newIRs.insert(newIRs.begin(), sp.begin(), sp.end());
        } else {
          newIRs.push_back(ir);
        }
      }
      _func->getIRs().clear();
      _func->getIRs().insert(_func->getIRs().begin(), newIRs.begin(),
                             newIRs.end());
    }
  } while (toContinueOuter);
}

std::unordered_map<Reg *, std::unordered_set<Reg *>>
FuncRegAlloc::calcConflictMap() {
  std::unordered_map<Reg *, std::unordered_set<int>> lifespans =
      calcLifespans();
  std::vector<std::unordered_set<Reg *>> regsInEackIR;
  regsInEackIR.reserve(_func->getIRs().size());
  for (int i = 0; i < _func->getIRs().size(); i++) {
    regsInEackIR.emplace_back();
  }
  for (const auto &lifespan : lifespans) {
    for (int id : lifespan.second) {
      regsInEackIR[id].insert(lifespan.first);
    }
  }
  std::unordered_map<Reg *, std::unordered_set<Reg *>> conflictMap;
  for (const auto &regs : regsInEackIR) {
    for (const auto reg : regs) {
      auto &conflicts = conflictMap.try_emplace(reg).first->second;
      conflicts.insert(regs.begin(), regs.end());
    }
  }
  for (auto &p : conflictMap) {
    p.second.erase(p.first);
  }
  return std::move(conflictMap);
}

void FuncRegAlloc::makeFrameInfo() {
  _funcParamSize = std::max(_func->maxFuncParamNum -
                                static_cast<int>(MReg::I_CALLER_REGS.size()),
                            0) *
                   8;
  _localSize = _func->getLocalSize();
  std::unordered_set<MReg *> usedICalleeRegs;
  std::unordered_set<MReg *> usedFCalleeRegs;
  _callAddrSize = 0;
  for (const auto ir : _func->getIRs()) {
    if (dynamic_cast<mir::CallMIR *>(ir)) {
      _callAddrSize = 8;
    }
    for (const auto reg : ir->getRegs()) {
      if (auto const mreg = dynamic_cast<MReg *>(reg)) {
        if (std::find(MReg::I_CALLEE_REGS.begin(), MReg::I_CALLEE_REGS.end(),
                      mreg) != MReg::I_CALLEE_REGS.end()) {
          usedICalleeRegs.insert(mreg);
          continue;
        }
        if (std::find(MReg::F_CALLEE_REGS.begin(), MReg::F_CALLEE_REGS.end(),
                      mreg) != MReg::F_CALLEE_REGS.end()) {
          usedFCalleeRegs.insert(mreg);
        }
      }
    }
  }
  for (const auto reg : MReg::I_CALLEE_REGS) {
    if (usedICalleeRegs.find(reg) != usedICalleeRegs.end()) {
      _iCalleeRegs.push_back(reg);
    }
  }
  for (const auto reg : MReg::F_CALLEE_REGS) {
    if (usedFCalleeRegs.find(reg) != usedFCalleeRegs.end()) {
      _fCalleeRegs.push_back(reg);
    }
  }
  _savedRegSize = (static_cast<int>(_iCalleeRegs.size()) +
                   static_cast<int>(_fCalleeRegs.size())) *
                  8;
  _alignSize = (_funcParamSize + _spillSize + _localSize + _paramInnerSize +
                _savedRegSize + _callAddrSize) %
               8;
}

void FuncRegAlloc::calcInOut(std::vector<Block *> &blocks) {
  bool toContinue;
  do {
    toContinue = false;
    for (int i = static_cast<int>(blocks.size()) - 1; i >= 0; i--) {
      Block *block = blocks[i];
      size_t sizeBefore = block->sizeOfInOut();
      block->calcIn();
      block->calcOut();
      size_t sizeAfter = block->sizeOfInOut();
      if (sizeBefore != sizeAfter) {
        toContinue = true;
      }
    }
  } while (toContinue);
}

void FuncRegAlloc::calcUseDef(std::vector<Block *> &blocks) const {
  auto &irs = _func->getIRs();
  for (const auto block : blocks) {
    for (int i = block->getBegin(); i < block->getEnd(); i++) {
      mir::MIR *mir = irs[i];
      if (auto const callMIR = dynamic_cast<mir::CallMIR *>(mir)) {
        int iSize = 0, fSize = 0;
        for (const auto arg : callMIR->func->getArgs()) {
          if (arg->getType() == ir::BasicType::FLOAT &&
              fSize < MReg::F_CALLER_REGS.size()) {
            if (!block->containsInDef(MReg::F_CALLER_REGS.at(fSize))) {
              block->addUse(MReg::F_CALLER_REGS.at(fSize));
            }
            fSize++;
          } else if (iSize < MReg::I_CALLER_REGS.size()) {
            if (!block->containsInDef(MReg::I_CALLER_REGS.at(iSize))) {
              block->addUse(MReg::I_CALLER_REGS.at(iSize));
            }
            iSize++;
          }
        }
        for (const auto reg : MReg::I_CALLER_REGS) {
          block->addDef(reg);
        }
        for (const auto reg : MReg::F_CALLER_REGS) {
          block->addDef(reg);
        }
      }
      for (const auto reg : mir->getRegs()) {
        if (!block->containsInDef(reg)) {
          block->addUse(reg);
        }
      }
      for (const auto reg : mir->getWrite()) {
        block->addDef(reg);
      }
    }
  }
}

void FuncRegAlloc::replaceFakeMIRs() {
  auto &irs = _func->getIRs();
  for (int i = 0; i < irs.size(); i++) {
    mir::MIR *mir = irs[i];
    if (const auto addRegLocalMIR = dynamic_cast<mir::AddRegLocalMIR *>(mir)) {
      int totalSize =
          _funcParamSize + _alignSize + _spillSize + addRegLocalMIR->imm;
      if (totalSize < 2048) {
        irs[i] = new mir::RriMIR(mir::RriMIR::ADDI, addRegLocalMIR->dest,
                                 MReg::SP, totalSize);
      } else {
        irs[i] = new mir::LiMIR(MReg::T0, totalSize);
        irs.insert(irs.begin() + i + 1,
                   new mir::RrrMIR(mir::RrrMIR::ADD, addRegLocalMIR->dest,
                                   MReg::SP, MReg::T0));
        i++;
      }
      continue;
    }
    if (const auto loadItemMIR = dynamic_cast<mir::LoadItemMIR *>(mir)) {
      int totalSize = 0, size = 0;
      const auto item = loadItemMIR->item;
      if (item == mir::LoadItemMIR::SPILL) {
        totalSize = _funcParamSize + _alignSize + loadItemMIR->imm;
      } else if (item == mir::LoadItemMIR::LOCAL) {
        size = 4;
        totalSize = _funcParamSize + _alignSize + _spillSize + loadItemMIR->imm;
      } else if (item == mir::LoadItemMIR::PARAM_INNER) {
        totalSize = _funcParamSize + _alignSize + _spillSize + _localSize +
                    _savedRegSize + loadItemMIR->imm;
      } else if (item == mir::LoadItemMIR::PARAM_OUTER) {
        totalSize = _funcParamSize + _alignSize + _spillSize + _localSize +
                    _savedRegSize + _paramInnerSize + _callAddrSize +
                    loadItemMIR->imm;
      }
      if (size == 0) {
        const auto type = loadItemMIR->dest->getType();
        if (type == ir::BasicType::FLOAT) {
          size = 4;
        } else if (type == ir::BasicType::I32) {
          size = 8;
        } else {
          throw std::runtime_error(
              "unexpected type in FuncRegAlloc::replaceFakeMIRs");
        }
      }
      if (totalSize < 2048) {
        irs[i] = new mir::LoadMIR(loadItemMIR->dest, MReg::SP, totalSize, size);
      } else {
        irs[i] = new mir::LiMIR(MReg::T0, totalSize);
        irs.insert(
            irs.begin() + i + 1,
            new mir::RrrMIR(mir::RrrMIR::ADD, MReg::T0, MReg::SP, MReg::T0));
        irs.insert(irs.begin() + i + 2,
                   new mir::LoadMIR(loadItemMIR->dest, MReg::T0, 0, size));
        i += 2;
      }
      continue;
    }
    if (const auto storeItemMIR = dynamic_cast<mir::StoreItemMIR *>(mir)) {
      int totalSize = 0, size = 0;
      const auto item = storeItemMIR->item;
      if (item == mir::StoreItemMIR::PARAM_CALL) {
        totalSize = storeItemMIR->imm;
      } else if (item == mir::StoreItemMIR::SPILL) {
        totalSize = _funcParamSize + _alignSize + storeItemMIR->imm;
      } else if (item == mir::StoreItemMIR::LOCAL) {
        size = 4;
        totalSize =
            _funcParamSize + _alignSize + _spillSize + storeItemMIR->imm;
      } else if (item == mir::StoreItemMIR::PARAM_INNER) {
        totalSize = _funcParamSize + _alignSize + _spillSize + _localSize +
                    _savedRegSize + storeItemMIR->imm;
      } else if (item == mir::StoreItemMIR::PARAM_OUTER) {
        totalSize = _funcParamSize + _alignSize + _spillSize + _localSize +
                    _savedRegSize + _paramInnerSize + _callAddrSize +
                    storeItemMIR->imm;
      }
      if (size == 0) {
        const auto type = storeItemMIR->src->getType();
        if (type == ir::BasicType::FLOAT) {
          size = 4;
        } else if (type == ir::BasicType::I32) {
          size = 8;
        } else {
          throw std::runtime_error(
              "unexpected type in FuncRegAlloc::replaceFakeMIRs");
        }
      }
      if (totalSize < 2048) {
        irs[i] =
            new mir::StoreMIR(storeItemMIR->src, MReg::SP, totalSize, size);
      } else {
        irs[i] = new mir::LiMIR(MReg::T0, totalSize);
        irs.insert(
            irs.begin() + i + 1,
            new mir::RrrMIR(mir::RrrMIR::ADD, MReg::T0, MReg::SP, MReg::T0));
        irs.insert(irs.begin() + i + 2,
                   new mir::StoreMIR(storeItemMIR->src, MReg::T0, 0, size));
        i += 2;
      }
    }
  }
}

std::unordered_map<VReg *, MReg *> FuncRegAlloc::calcVRegToMReg() {
  std::unordered_map<Reg *, std::unordered_set<Reg *>> conflictMap =
      calcConflictMap();
  std::unordered_map<VReg *, MReg *> vRegToMReg;
  for (const auto &p : conflictMap) {
    if (auto const vreg = dynamic_cast<VReg *>(p.first)) {
      const std::vector<MReg *> &regs =
          vreg->getType() == ir::BasicType::FLOAT ? MReg::F_REGS : MReg::I_REGS;
      std::unordered_set<MReg *> usedRegs;
      for (const auto reg : conflictMap.at(vreg)) {
        if (auto const vreg1 = dynamic_cast<VReg *>(reg)) {
          MReg *mreg = vRegToMReg[vreg1];
          if (mreg) {
            usedRegs.insert(mreg);
          }
          continue;
        }
        if (auto const mreg = dynamic_cast<MReg *>(reg)) {
          usedRegs.insert(mreg);
          continue;
        }
        throw std::runtime_error(
            "unexpected reg type in FuncRegAlloc::calcVRegToMReg");
      }
      for (const auto mreg : regs) {
        if (usedRegs.find(mreg) != usedRegs.end()) {
          continue;
        }
        vRegToMReg[vreg] = mreg;
        break;
      }
    }
  }
  return std::move(vRegToMReg);
}

} // namespace reg
