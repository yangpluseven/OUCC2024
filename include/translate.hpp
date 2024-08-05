#ifndef MIR_TRANSLATE_HPP
#define MIR_TRANSLATE_HPP

#include "mir.hpp"
#include "register.hpp"
#include <bitset>
#include <limits>
#include <vector>

namespace mir {

class MIROpUtil {
public:
  static void loadImmToIReg(std::vector<MIR *> &irs, reg::Reg *reg, int imm) {
    irs.push_back(new LoadImm(reg, imm));
  }

  static void loadImmToFReg(std::vector<MIR *> &irs, reg::Reg *reg, int imm) {
    auto midReg = new reg::Virtual(ir::BasicType::I32);
    loadImmToIReg(irs, midReg, imm);
    irs.push_back(new RR(RR::MV, reg, midReg));
  }

  static void loadImmToReg(std::vector<MIR *> &irs, reg::Reg *reg, int imm) {
    if (reg->getType() != ir::BasicType::I32) {
      loadImmToFReg(irs, reg, imm);
    } else {
      loadImmToIReg(irs, reg, imm);
    }
  }

  static void loadImmToReg(std::vector<MIR *> &irs, reg::Reg *reg, float imm) {
    if (reg->getType() != ir::BasicType::FLOAT) {
      throw std::runtime_error("reg is not float in MIROpHelper::loadImmToReg");
    }
    loadImmToReg(irs, reg, *reinterpret_cast<int *>(&imm));
  }

  // TODO check following functions
  static int bitcount(int x) {
    return static_cast<int>(std::bitset<32>(x).count());
  }

  static int leadingZeros(int x) {
    constexpr int bitSize = std::numeric_limits<int>::digits + 1;
    if (x == 0) {
      return bitSize;
    }
    std::bitset<bitSize> bits(x);
    int count = 0;
    for (int i = bitSize - 1; i >= 0; --i) {
      if (bits[i] == 0) {
        count++;
      } else {
        break;
      }
    }
    return count;
  }

  static int trailingZeros(int x) {
    constexpr int bitSize = std::numeric_limits<int>::digits + 1;
    if (x == 0) {
      return bitSize;
    }
    std::bitset<bitSize> bits(x);
    int count = 0;
    for (int i = 0; i < bitSize; ++i) {
      if (bits[i] == 0) {
        count++;
      } else {
        break;
      }
    }
    return count;
  }
};

class MIRBinTranslator {
private:
  static void transAddRegImmF(std::vector<MIR *> &irs, reg::Virtual *target,
                              reg::Virtual *vsrc, float imm);
  static void transAddRegRegF(std::vector<MIR *> &irs, reg::Virtual *target,
                              reg::Virtual *vsrc1, reg::Virtual *vsrc2) {
    irs.push_back(new RRR(RRR::ADD, target, vsrc1, vsrc2));
  }
  static void transAddRegRegI(std::vector<MIR *> &irs, reg::Virtual *target,
                              reg::Virtual *vsrc1, reg::Virtual *vsrc2) {
    irs.push_back(new RRR(RRR::ADD, target, vsrc1, vsrc2));
  }
  static void transDivImmRegF(std::vector<MIR *> &irs, reg::Virtual *target,
                              float imm, reg::Virtual *vsrc);
  static void transDivImmRegI(std::vector<MIR *> &irs, reg::Virtual *target,
                              int imm, reg::Virtual *vsrc);
  static void transDivRegImmF(std::vector<MIR *> &irs, reg::Virtual *target,
                              reg::Virtual *vsrc, float imm);
  static void transDivRegImmI(std::vector<MIR *> &irs, reg::Virtual *target,
                              reg::Virtual *vsrc, int imm);

  static void transDivRegRegF(std::vector<MIR *> &irs, reg::Virtual *target,
                              reg::Virtual *vsrc1, reg::Virtual *vsrc2) {
    irs.push_back(new RRR(RRR::DIV, target, vsrc1, vsrc2));
  }
  static void transDivRegRegI(std::vector<MIR *> &irs, reg::Virtual *target,
                              reg::Virtual *vsrc1, reg::Virtual *vsrc2) {
    irs.push_back(new RRR(RRR::DIVW, target, vsrc1, vsrc2));
  }
  static void transModImmReg(std::vector<MIR *> &irs, reg::Virtual *target,
                             int imm, reg::Virtual *vsrc);
  static void transModRegImm(std::vector<MIR *> &irs, reg::Virtual *target,
                             reg::Virtual *vsrc, int imm);
  static void transModRegReg(std::vector<MIR *> &irs, reg::Virtual *target,
                             reg::Virtual *vsrc1, reg::Virtual *vsrc2) {
    irs.push_back(new RRR(RRR::REMW, target, vsrc1, vsrc2));
  }
  static void transMulRegImmF(std::vector<MIR *> &irs, reg::Virtual *target,
                              reg::Virtual *vsrc, float imm);
  static void transMulRegImmI(std::vector<MIR *> &irs, reg::Virtual *target,
                              reg::Virtual *vsrc, int imm);
  static void transMulRegRegF(std::vector<MIR *> &irs, reg::Virtual *target,
                              reg::Virtual *vsrc1, reg::Virtual *vsrc2) {
    irs.push_back(new RRR(RRR::MUL, target, vsrc1, vsrc2));
  }
  static void transMulRegRegI(std::vector<MIR *> &irs, reg::Virtual *target,
                              reg::Virtual *vsrc1, reg::Virtual *vsrc2) {
    irs.push_back(new RRR(RRR::MULW, target, vsrc1, vsrc2));
  }
  static void transSubImmRegF(std::vector<MIR *> &irs, reg::Virtual *target,
                              float imm, reg::Virtual *vsrc);
  static void transSubImmRegI(std::vector<MIR *> &irs, reg::Virtual *target,
                              int imm, reg::Virtual *vsrc);
  static void transSubRegImmF(std::vector<MIR *> &irs, reg::Virtual *target,
                              reg::Virtual *vsrc, float imm);
  static void transSubRegImmI(std::vector<MIR *> &irs, reg::Virtual *target,
                              reg::Virtual *vsrc, int imm);
  static void transSubRegRegF(std::vector<MIR *> &irs, reg::Virtual *target,
                              reg::Virtual *vsrc1, reg::Virtual *vsrc2) {
    irs.push_back(new RRR(RRR::SUB, target, vsrc1, vsrc2));
  }
  static void transSubRegRegI(std::vector<MIR *> &irs, reg::Virtual *target,
                              reg::Virtual *vsrc1, reg::Virtual *vsrc2) {
    irs.push_back(new RRR(RRR::SUBW, target, vsrc1, vsrc2));
  }

public:
  static void transAddRegImmI(std::vector<MIR *> &irs, reg::Virtual *target,
                              reg::Virtual *vsrc, int imm);
  static void transBinImmReg(
      std::vector<MIR *> &irs,
      const std::unordered_map<ir::Instruction *, reg::Virtual *> &instRegMap,
      ir::BinaryInst *binOp, ir::ConstantNumber *value, reg::Virtual *reg);
  static void transBinRegImm(
      std::vector<MIR *> &irs,
      const std::unordered_map<ir::Instruction *, reg::Virtual *> &instRegMap,
      ir::BinaryInst *binOp, reg::Virtual *reg, ir::ConstantNumber *value);
  static void transBinRegReg(
      std::vector<MIR *> &irs,
      const std::unordered_map<ir::Instruction *, reg::Virtual *> &instRegMap,
      ir::BinaryInst *binOp, reg::Virtual *reg1, reg::Virtual *reg2);
};

class MIRTranslator {
public:
  static void branch(
      ir::BranchInst *branchInst, std::vector<MIR *> &mirs,
      std::unordered_map<ir::Instruction *, reg::Virtual *> const &instRegMap);
  static void binary(
      ir::BinaryInst *binOp, std::vector<MIR *> &mirs,
      std::unordered_map<ir::Instruction *, reg::Virtual *> const &instRegMap,
      std::unordered_map<ir::Argument *, std::pair<bool, int>> const
          &argOffsets);
  static int
  call(ir::CallInst *callInst, std::vector<MIR *> &mirs,
       std::unordered_map<ir::Instruction *, reg::Virtual *> const &instRegMap,
       std::unordered_map<ir::Argument *, std::pair<bool, int>> const
           &argOffsets,
       std::unordered_map<ir::AllocaInst *, int> const &localOffsets);
  static void getPtr(
      ir::GetPtrInst *getPtrInst, std::vector<MIR *> &mirs,
      std::unordered_map<ir::Instruction *, reg::Virtual *> const &instRegMap,
      std::unordered_map<ir::Argument *, std::pair<bool, int>> const
          &argOffsets,
      std::unordered_map<ir::AllocaInst *, int> const &localOffsets);
  static void
  load(ir::LoadInst *loadInst, std::vector<MIR *> &mirs,
       std::unordered_map<ir::Instruction *, reg::Virtual *> const &instRegMap,
       std::unordered_map<ir::Argument *, std::pair<bool, int>> const
           &argOffsets,
       std::unordered_map<ir::AllocaInst *, int> const &localOffsets);
  static void
  ret(ir::RetInst *retInst, std::vector<MIR *> &mirs,
      std::unordered_map<ir::Instruction *, reg::Virtual *> const &instRegMap,
      std::unordered_map<ir::Argument *, std::pair<bool, int>> const
          &argOffsets,
      Label *retLabel);
  static void
  store(ir::StoreInst *storeInst, std::vector<MIR *> &mirs,
        std::unordered_map<ir::Instruction *, reg::Virtual *> const &instRegMap,
        std::unordered_map<ir::Argument *, std::pair<bool, int>> const
            &argOffsets,
        std::unordered_map<ir::AllocaInst *, int> const &localOffsets);
  static void bitCast(
      ir::BitCastInst *bitCastInst, std::vector<MIR *> &mirs,
      std::unordered_map<ir::Instruction *, reg::Virtual *> const &instRegMap,
      std::unordered_map<ir::AllocaInst *, int> const &localOffsets);
  static void
  iCmp(ir::ICmpInst *iCmpInst, std::vector<MIR *> &mirs,
       std::unordered_map<ir::Instruction *, reg::Virtual *> const &instRegMap,
       std::unordered_map<ir::Argument *, std::pair<bool, int>> const
           &argOffsets,
       std::unordered_map<ir::AllocaInst *, int> const &localOffsets);
  static void
  fCmp(ir::FCmpInst *fCmpInst, std::vector<MIR *> &mirs,
       std::unordered_map<ir::Instruction *, reg::Virtual *> const &instRegMap,
       std::unordered_map<ir::Argument *, std::pair<bool, int>> const
           &argOffsets,
       std::unordered_map<ir::AllocaInst *, int> const &localOffsets);
};

} // namespace mir

#endif // MIR_TRANSLATE_HPP
