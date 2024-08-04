#ifndef REG_REGISTER_HPP
#define REG_REGISTER_HPP

#include "type.hpp"
#include <stdexcept>

namespace reg {

class Reg {
protected:
  ir::Type *type;

  explicit Reg(ir::Type *type) : type(type) {}

public:
  virtual ~Reg() = default;
  [[nodiscard]] ir::Type *getType() const { return type; }
  [[nodiscard]] virtual std::string str() const = 0;
};

class Virtual : public Reg {
private:
  static int _counter;
  int _id;

public:
  explicit Virtual(ir::Type *type) : Reg(type), _id(_counter++) {}

  [[nodiscard]] std::string str() const override {
    if (type == ir::BasicType::I32) {
      return "$i" + std::to_string(_id);
    }
    if (type == ir::BasicType::FLOAT) {
      return "$f" + std::to_string(_id);
    }
    throw std::runtime_error("Invalid type");
  };
};

class Machine : public Reg {
private:
  std::string _name;

public:
  static Machine *const ZERO;
  static Machine *const RA;
  static Machine *const SP;
  static Machine *const GP;
  static Machine *const TP;
  static Machine *const T0;
  static Machine *const T1;
  static Machine *const T2;
  static Machine *const S0;
  static Machine *const S1;
  static Machine *const A0;
  static Machine *const A1;
  static Machine *const A2;
  static Machine *const A3;
  static Machine *const A4;
  static Machine *const A5;
  static Machine *const A6;
  static Machine *const A7;
  static Machine *const S2;
  static Machine *const S3;
  static Machine *const S4;
  static Machine *const S5;
  static Machine *const S6;
  static Machine *const S7;
  static Machine *const S8;
  static Machine *const S9;
  static Machine *const S10;
  static Machine *const S11;
  static Machine *const T3;
  static Machine *const T4;
  static Machine *const T5;
  static Machine *const T6;
  static Machine *const FA0;
  static Machine *const FA1;
  static Machine *const FA2;
  static Machine *const FA3;
  static Machine *const FA4;
  static Machine *const FA5;
  static Machine *const FA6;
  static Machine *const FA7;
  static Machine *const FS0;
  static Machine *const FS1;
  static Machine *const FS2;
  static Machine *const FS3;
  static Machine *const FS4;
  static Machine *const FS5;
  static Machine *const FS6;
  static Machine *const FS7;
  static Machine *const FS8;
  static Machine *const FS9;
  static Machine *const FS10;
  static Machine *const FS11;
  static Machine *const FT0;
  static Machine *const FT1;
  static Machine *const FT2;
  static Machine *const FT3;
  static Machine *const FT4;
  static Machine *const FT5;
  static Machine *const FT6;
  static Machine *const FT7;
  static Machine *const FT8;
  static Machine *const FT9;
  static Machine *const FT10;
  static Machine *const FT11;

  static const std::vector<Machine *> I_REGS;
  static const std::vector<Machine *> F_REGS;
  static const std::vector<Machine *> I_CALLER_REGS;
  static const std::vector<Machine *> F_CALLER_REGS;
  static const std::vector<Machine *> I_CALLEE_REGS;
  static const std::vector<Machine *> F_CALLEE_REGS;

  Machine(ir::Type *type, std::string name) : Reg(type), _name(std::move(name)) {}

  [[nodiscard]] std::string str() const override { return _name; };
};

} // namespace reg

#endif // REG_REGISTER_HPP