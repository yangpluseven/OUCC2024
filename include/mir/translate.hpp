#ifndef MIR_TRANSLATE_HPP
#define MIR_TRANSLATE_HPP

#include "mir.hpp"
#include "reg/register.hpp"
#include <bitset>
#include <limits>
#include <vector>

namespace mir {

class MIROpUtil {
public:
  static void loadImmToIReg(std::vector<MIR *> &irs, reg::Reg *reg, int imm) {
    irs.push_back(new LiMIR(reg, imm));
  }

  static void loadImmToFReg(std::vector<MIR *> &irs, reg::Reg *reg, int imm) {
    auto midReg = new reg::VReg(ir::BasicType::I32);
    loadImmToIReg(irs, midReg, imm);
    irs.push_back(new RrMIR(RrMIR::MV, reg, midReg));
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
    constexpr int bitSize = std::numeric_limits<int>::digits;
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
    constexpr int bitSize = std::numeric_limits<int>::digits;
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
  static void transAddRegImmF(std::vector<MIR *> &irs, reg::VReg *target,
                              reg::VReg *vsrc, float imm);
  static void transAddRegRegF(std::vector<MIR *> &irs, reg::VReg *target,
                              reg::VReg *vsrc1, reg::VReg *vsrc2) {
    irs.push_back(new RrrMIR(RrrMIR::ADD, target, vsrc1, vsrc2));
  }
  static void transAddRegRegI(std::vector<MIR *> &irs, reg::VReg *target,
                              reg::VReg *vsrc1, reg::VReg *vsrc2) {
    irs.push_back(new RrrMIR(RrrMIR::ADD, target, vsrc1, vsrc2));
  }
  static void transDivImmRegF(std::vector<MIR *> &irs, reg::VReg *target,
                              float imm, reg::VReg *vsrc);
  static void transDivImmRegI(std::vector<MIR *> &irs, reg::VReg *target,
                              int imm, reg::VReg *vsrc);
  static void transDivRegImmF(std::vector<MIR *> &irs, reg::VReg *target,
                              reg::VReg *vsrc, float imm);
  static void transDivRegImmI(std::vector<MIR *> &irs, reg::VReg *target,
                              reg::VReg *vsrc, int imm);

  static void transDivRegRegF(std::vector<MIR *> &irs, reg::VReg *target,
                              reg::VReg *vsrc1, reg::VReg *vsrc2) {
    irs.push_back(new RrrMIR(RrrMIR::DIV, target, vsrc1, vsrc2));
  }
  static void transDivRegRegI(std::vector<MIR *> &irs, reg::VReg *target,
                              reg::VReg *vsrc1, reg::VReg *vsrc2) {
    irs.push_back(new RrrMIR(RrrMIR::DIVW, target, vsrc1, vsrc2));
  }
  static void transModImmReg(std::vector<MIR *> &irs, reg::VReg *target,
                             int imm, reg::VReg *vsrc);
  static void transModRegImm(std::vector<MIR *> irs, reg::VReg *target,
                             reg::VReg *vsrc, int imm);
  static void transModRegReg(std::vector<MIR *> &irs, reg::VReg *target,
                             reg::VReg *vsrc1, reg::VReg *vsrc2) {
    irs.push_back(new RrrMIR(RrrMIR::REMW, target, vsrc1, vsrc2));
  }
  static void transMulRegImmF(std::vector<MIR *> &irs, reg::VReg *target,
                              reg::VReg *vsrc, float imm);
  static void transMulRegImmI(std::vector<MIR *> &irs, reg::VReg *target,
                              reg::VReg *vsrc, int imm);
  static void transMulRegRegF(std::vector<MIR *> &irs, reg::VReg *target,
                              reg::VReg *vsrc1, reg::VReg *vsrc2) {
    irs.push_back(new RrrMIR(RrrMIR::MUL, target, vsrc1, vsrc2));
  }
  static void transMulRegRegI(std::vector<MIR *> &irs, reg::VReg *target,
                              reg::VReg *vsrc1, reg::VReg *vsrc2) {
    irs.push_back(new RrrMIR(RrrMIR::MULW, target, vsrc1, vsrc2));
  }
  static void transSubImmRegF(std::vector<MIR *> &irs, reg::VReg *target,
                              float imm, reg::VReg *vsrc);
  static void transSubImmRegI(std::vector<MIR *> &irs, reg::VReg *target,
                              int imm, reg::VReg *vsrc);
  static void transSubRegImmF(std::vector<MIR *> &irs, reg::VReg *target,
                              reg::VReg *vsrc, float imm);
  static void transSubRegImmI(std::vector<MIR *> &irs, reg::VReg *target,
                              reg::VReg *vsrc, int imm);
  static void transSubRegRegF(std::vector<MIR *> &irs, reg::VReg *target,
                              reg::VReg *vsrc1, reg::VReg *vsrc2) {
    irs.push_back(new RrrMIR(RrrMIR::SUB, target, vsrc1, vsrc2));
  }
  static void transSubRegRegI(std::vector<MIR *> &irs, reg::VReg *target,
                              reg::VReg *vsrc1, reg::VReg *vsrc2) {
    irs.push_back(new RrrMIR(RrrMIR::SUBW, target, vsrc1, vsrc2));
  }

public:
  static void transAddRegImmI(std::vector<MIR *> &irs, reg::VReg *target,
                              reg::VReg *vsrc, int imm);
  static void
  transBinImmReg(std::vector<MIR *> &irs,
                 const std::unordered_map<ir::Instruction *, reg::VReg *> &instRegMap,
                 ir::BinaryOperator *binOp, ir::ConstantNumber *value,
                 reg::VReg *reg);
  static void
  transBinRegImm(std::vector<MIR *> &irs,
                 const std::unordered_map<ir::Instruction *, reg::VReg *> &instRegMap,
                 ir::BinaryOperator *binOp, reg::VReg *reg,
                 ir::ConstantNumber *value);
  static void
  transBinRegReg(std::vector<MIR *> &irs,
                 const std::unordered_map<ir::Instruction *, reg::VReg *> &instRegMap,
                 ir::BinaryOperator *binOp, reg::VReg *reg1, reg::VReg *reg2);
};

} // namespace mir

#endif // MIR_TRANSLATE_HPP
