#ifndef REG_REGISTER_HPP
#define REG_REGISTER_HPP
#include "ir/type.hpp"
#include <stdexcept>

namespace reg {

class Reg {
protected:
  ir::Type *type;

  explicit Reg(ir::Type *type) : type(type) {
  }

public:
  virtual ~Reg() = default;
  [[nodiscard]] ir::Type *getType() const { return type; }
  [[nodiscard]] virtual std::string toString() const = 0;
};

class VReg : public Reg {
private:
  static int _counter;
  int _id;

public:
  explicit VReg(ir::Type *type) : Reg(type), _id(_counter++) {
  }

  [[nodiscard]] std::string toString() const override {
    if (type == ir::BasicType::I32) {
      return "$i" + std::to_string(_id);
    }
    if (type == ir::BasicType::FLOAT) {
      return "$f" + std::to_string(_id);
    }
    throw std::runtime_error("Invalid type");
  };
};

int VReg::_counter = 0;

class MReg : public Reg {
private:
  std::string _name;

public:
  static MReg *const ZERO;
  static MReg *const RA;
  static MReg *const SP;
  static MReg *const GP;
  static MReg *const TP;
  static MReg *const T0;
  static MReg *const T1;
  static MReg *const T2;
  static MReg *const S0;
  static MReg *const S1;
  static MReg *const A0;
  static MReg *const A1;
  static MReg *const A2;
  static MReg *const A3;
  static MReg *const A4;
  static MReg *const A5;
  static MReg *const A6;
  static MReg *const A7;
  static MReg *const S2;
  static MReg *const S3;
  static MReg *const S4;
  static MReg *const S5;
  static MReg *const S6;
  static MReg *const S7;
  static MReg *const S8;
  static MReg *const S9;
  static MReg *const S10;
  static MReg *const S11;
  static MReg *const T3;
  static MReg *const T4;
  static MReg *const T5;
  static MReg *const T6;
  static MReg *const FA0;
  static MReg *const FA1;
  static MReg *const FA2;
  static MReg *const FA3;
  static MReg *const FA4;
  static MReg *const FA5;
  static MReg *const FA6;
  static MReg *const FA7;
  static MReg *const FS0;
  static MReg *const FS1;
  static MReg *const FS2;
  static MReg *const FS3;
  static MReg *const FS4;
  static MReg *const FS5;
  static MReg *const FS6;
  static MReg *const FS7;
  static MReg *const FS8;
  static MReg *const FS9;
  static MReg *const FS10;
  static MReg *const FS11;
  static MReg *const FT0;
  static MReg *const FT1;
  static MReg *const FT2;
  static MReg *const FT3;
  static MReg *const FT4;
  static MReg *const FT5;
  static MReg *const FT6;
  static MReg *const FT7;
  static MReg *const FT8;
  static MReg *const FT9;
  static MReg *const FT10;
  static MReg *const FT11;
  static const std::vector<MReg *> iRegs;
  static const std::vector<MReg *> fRegs;
  static const std::vector<MReg *> callerIRegs;
  static const std::vector<MReg *> callerFRegs;
  static const std::vector<MReg *> calleeIRegs;
  static const std::vector<MReg *> calleeFRegs;

  MReg(ir::Type *type, std::string name) : Reg(type), _name(std::move(name)) {
  }

  [[nodiscard]] std::string toString() const override {
    return _name;
  };
};

}

#endif //REG_REGISTER_HPP