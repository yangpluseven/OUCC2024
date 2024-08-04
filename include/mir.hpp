#ifndef MIR_MIR_HPP
#define MIR_MIR_HPP
#include "basic_block.hpp"
#include "global_variable.hpp"
#include "register.hpp"

#include <unordered_map>

namespace mir {

class MIR {
private:
  static int _counter;
  int _id;

public:
  MIR() : _id(_counter++) {}

  virtual ~MIR() = default;

  virtual std::vector<reg::Reg *> getRead() { return {}; }

  virtual std::vector<reg::Reg *> getWrite() { return {}; }

  virtual MIR *
  replaceReg(const std::unordered_map<reg::Virtual *, reg::Machine *> &replaceMap) {
    return this;
  }

  virtual std::vector<MIR *> spill(reg::Reg *reg, int offset) { return {this}; }

  [[nodiscard]] virtual std::string str() const = 0;

  std::vector<reg::Reg *> getRegs() {
    std::vector<reg::Reg *> regs;
    auto rd = getRead();
    auto wr = getWrite();
    regs.insert(regs.end(), rd.begin(), rd.end());
    regs.insert(regs.end(), wr.begin(), wr.end());
    return regs;
  }

  bool operator==(const MIR &rhs) const { return _id == rhs._id; }
};

class RegAddImm : public MIR {
public:
  reg::Reg *dest;
  int imm;

  RegAddImm(reg::Reg *dest, int imm) : dest(dest), imm(imm) {}

  std::vector<reg::Reg *> getWrite() override { return {dest}; }

  MIR *replaceReg(
      const std::unordered_map<reg::Virtual *, reg::Machine *> &replaceMap) override {
    reg::Reg *newDest = dest;
    if (auto vdest = dynamic_cast<reg::Virtual *>(dest)) {
      if (replaceMap.find(vdest) != replaceMap.end())
        newDest = replaceMap.at(vdest);
    }
    return new RegAddImm(newDest, imm);
  }

  std::vector<MIR *> spill(reg::Reg *reg, int offset) override;

  [[nodiscard]] std::string str() const override {
    return "add\t" + dest->str() + ", $local, #" + std::to_string(imm);
  }
};

class Branch : public MIR {
public:
  enum Op { NUL, EQ, NE, LT, LE, GT, GE };

  Op op = NUL;
  reg::Reg *src1, *src2;
  ir::BasicBlock *block;

  Branch(Op op, reg::Reg *src1, reg::Reg *src2, ir::BasicBlock *block)
      : op(op), src1(src1), src2(src2), block(block) {}

  std::vector<reg::Reg *> getRead() override {
    if (op == NUL) {
      return {};
    }
    return {src1, src2};
  }

  MIR *replaceReg(
      const std::unordered_map<reg::Virtual *, reg::Machine *> &replaceMap) override {
    reg::Reg *newSrc1 = src1, *newSrc2 = src2;
    if (auto vsrc1 = dynamic_cast<reg::Virtual *>(src1)) {
      if (replaceMap.find(vsrc1) != replaceMap.end())
        newSrc1 = replaceMap.at(vsrc1);
    }
    if (auto vsrc2 = dynamic_cast<reg::Virtual *>(src2)) {
      if (replaceMap.find(vsrc2) != replaceMap.end())
        newSrc2 = replaceMap.at(vsrc2);
    }
    return new Branch(op, newSrc1, newSrc2, block);
  }

  std::vector<MIR *> spill(reg::Reg *reg, int offset) override;

  [[nodiscard]] bool hasCond() const { return op != NUL; }

  [[nodiscard]] std::string str() const override {
    if (op == NUL) {
      return "j\t." + block->getName();
    }
    return "b" + _opToString(op) + "\t" + src1->str() + ", " +
           src2->str() + ", ." + block->getName();
  }

private:
  static std::string _opToString(Op op) noexcept {
    switch (op) {
    case NUL:
      return "{null}";
    case EQ:
      return "eq";
    case NE:
      return "ne";
    case LT:
      return "lt";
    case LE:
      return "le";
    case GT:
      return "gt";
    case GE:
      return "ge";
    default:
      return "incomplete{_opToString}";
    }
  }
};

class Call : public MIR {
public:
  ir::Function *func;

  explicit Call(ir::Function *func) : func(func) {}

  std::vector<reg::Reg *> getRead() override {
    std::vector<reg::Reg *> regs;
    int isize = 0, fsize = 0;
    for (auto arg : func->getArgs()) {
      if (arg->getType() == ir::BasicType::FLOAT) {
        if (fsize < reg::Machine::F_CALLER_REGS.size()) {
          regs.push_back(reg::Machine::F_CALLER_REGS.at(fsize));
        }
        fsize++;
      } else {
        if (isize < reg::Machine::I_CALLER_REGS.size()) {
          regs.push_back(reg::Machine::I_CALLER_REGS.at(isize));
        }
        isize++;
      }
    }
    return regs;
  };

  std::vector<reg::Reg *> getWrite() override {
    std::vector<reg::Reg *> regs;
    regs.insert(regs.end(), reg::Machine::I_CALLER_REGS.begin(),
                reg::Machine::I_CALLER_REGS.end());
    regs.insert(regs.end(), reg::Machine::F_CALLER_REGS.begin(),
                reg::Machine::F_CALLER_REGS.end());
    return regs;
  }

  [[nodiscard]] std::string str() const override {
    return "call\t" + func->getRawName();
  }
};

class Label : public MIR {
public:
  ir::BasicBlock *block;

  explicit Label(ir::BasicBlock *block) : block(block) {}

  [[nodiscard]] ir::BasicBlock *getBlock() const { return block; }

  [[nodiscard]] std::string str() const override {
    return "." + block->getName() + ":";
  }
};

class LoadImm : public MIR {
public:
  reg::Reg *dest;
  int imm;

  LoadImm(reg::Reg *dest, int imm) : dest(dest), imm(imm) {}

  std::vector<reg::Reg *> getWrite() override { return {dest}; }

  MIR *replaceReg(
      const std::unordered_map<reg::Virtual *, reg::Machine *> &replaceMap) override {
    reg::Reg *newDest = dest;
    if (auto vdest = dynamic_cast<reg::Virtual *>(dest)) {
      if (replaceMap.find(vdest) != replaceMap.end())
        newDest = replaceMap.at(vdest);
    }
    return new LoadImm(newDest, imm);
  }

  std::vector<MIR *> spill(reg::Reg *reg, int offset) override;

  [[nodiscard]] std::string str() const override {
    return "li\t" + dest->str() + ", " + std::to_string(imm);
  }
};

class LLA : public MIR {
public:
  reg::Reg *dest;
  ir::GlobalVariable *global;

  LLA(reg::Reg *dest, ir::GlobalVariable *global)
      : dest(dest), global(global) {}

  std::vector<reg::Reg *> getWrite() override { return {dest}; }

  MIR *replaceReg(
      const std::unordered_map<reg::Virtual *, reg::Machine *> &replaceMap) override {
    reg::Reg *newDest = dest;
    if (auto vdest = dynamic_cast<reg::Virtual *>(dest)) {
      if (replaceMap.find(vdest) != replaceMap.end())
        newDest = replaceMap.at(vdest);
    }
    return new LLA(newDest, global);
  }

  std::vector<MIR *> spill(reg::Reg *reg, int offset) override;

  [[nodiscard]] std::string str() const override {
    return "lla\t" + dest->str() + ", " + global->getRawName();
  }
};

class LoadFrom : public MIR {
public:
  enum From { SPILL, INNER_PARAM, OUTER_PARAM, LOCAL };

  From from;
  reg::Reg *dest;
  int imm;

  LoadFrom(From item, reg::Reg *dest, int imm)
      : from(item), dest(dest), imm(imm) {}

  std::vector<reg::Reg *> getWrite() override { return {dest}; }

  MIR *replaceReg(
      const std::unordered_map<reg::Virtual *, reg::Machine *> &replaceMap) override {
    reg::Reg *newDest = dest;
    if (auto vdest = dynamic_cast<reg::Virtual *>(dest)) {
      if (replaceMap.find(vdest) != replaceMap.end()) {
        newDest = replaceMap.at(vdest);
      }
    }
    return new LoadFrom(from, newDest, imm);
  }

  std::vector<MIR *> spill(reg::Reg *reg, int offset) override;

  [[nodiscard]] std::string str() const override {
    return "load\t" + dest->str() + ", " + std::to_string(imm) + "($" +
           _itemToString(from) + ")";
  }

private:
  static std::string _itemToString(From v) noexcept {
    switch (v) {
    case SPILL:
      return "spill";
    case INNER_PARAM:
      return "inner";
    case OUTER_PARAM:
      return "outer";
    case LOCAL:
      return "local";
    default:
      return "incomplete{_itemToString}";
    }
  }
};

class Load : public MIR {
public:
  reg::Reg *dest, *src;
  int imm, size;

  Load(reg::Reg *dest, reg::Reg *src, int imm, int size)
      : dest(dest), src(src), imm(imm), size(size) {}

  std::vector<reg::Reg *> getRead() override { return {src}; }

  std::vector<reg::Reg *> getWrite() override { return {dest}; }

  MIR *replaceReg(
      const std::unordered_map<reg::Virtual *, reg::Machine *> &replaceMap) override {
    reg::Reg *newDest = dest, *newSrc = src;
    if (auto vdest = dynamic_cast<reg::Virtual *>(dest)) {
      if (replaceMap.find(vdest) != replaceMap.end())
        newDest = replaceMap.at(vdest);
    }
    if (auto vsrc = dynamic_cast<reg::Virtual *>(src)) {
      if (replaceMap.find(vsrc) != replaceMap.end())
        newSrc = replaceMap.at(vsrc);
    }
    return new Load(newDest, newSrc, imm, size);
  }

  std::vector<MIR *> spill(reg::Reg *reg, int offset) override;
  [[nodiscard]] std::string str() const override;
};

class RR : public MIR {
public:
  enum Op { CVT, FABS, MV, NEG, SEQZ, SNEZ };

  Op op;
  reg::Reg *dest, *src;

  RR(Op op, reg::Reg *dest, reg::Reg *src) : op(op), dest(dest), src(src) {}

  std::vector<reg::Reg *> getRead() override { return {src}; }

  std::vector<reg::Reg *> getWrite() override { return {dest}; }

  MIR *replaceReg(
      const std::unordered_map<reg::Virtual *, reg::Machine *> &replaceMap) override {
    reg::Reg *newDest = dest, *newSrc = src;
    if (auto vdest = dynamic_cast<reg::Virtual *>(dest)) {
      if (replaceMap.find(vdest) != replaceMap.end())
        newDest = replaceMap.at(vdest);
    }
    if (auto vsrc = dynamic_cast<reg::Virtual *>(src)) {
      if (replaceMap.find(vsrc) != replaceMap.end())
        newSrc = replaceMap.at(vsrc);
    }
    return new RR(op, newDest, newSrc);
  }

  std::vector<MIR *> spill(reg::Reg *reg, int offset) override;
  [[nodiscard]] std::string str() const override;

private:
  static std::string _opToString(Op v) noexcept {
    switch (v) {
    case CVT:
      return "cvt";
    case FABS:
      return "fabs";
    case MV:
      return "mv";
    case NEG:
      return "neg";
    case SEQZ:
      return "seqz";
    case SNEZ:
      return "snez";
    default:
      return "incomplete{_opToString}";
    }
  }
};

class RRI : public MIR {
public:
  enum Op { ADDI, ANDI, SLLIW, SRAIW, SRLI, SRLIW, XORI, SLTI };

  Op op;
  reg::Reg *dest, *src;
  int imm;

  RRI(Op op, reg::Reg *dest, reg::Reg *src, int imm)
      : op(op), dest(dest), src(src), imm(imm) {}

  std::vector<reg::Reg *> getRead() override { return {src}; }

  std::vector<reg::Reg *> getWrite() override { return {dest}; }

  MIR *replaceReg(
      const std::unordered_map<reg::Virtual *, reg::Machine *> &replaceMap) override {
    reg::Reg *newDest = dest, *newSrc = src;
    if (auto vdest = dynamic_cast<reg::Virtual *>(dest)) {
      if (replaceMap.find(vdest) != replaceMap.end())
        newDest = replaceMap.at(vdest);
    }
    if (auto vsrc = dynamic_cast<reg::Virtual *>(src)) {
      if (replaceMap.find(vsrc) != replaceMap.end())
        newSrc = replaceMap.at(vsrc);
    }
    return new RRI(op, newDest, newSrc, imm);
  }

  std::vector<MIR *> spill(reg::Reg *reg, int offset) override;

  [[nodiscard]] std::string str() const override {
    return _opToString(op) + "\t" + dest->str() + ", " + src->str() +
           ", " + std::to_string(imm);
  }

private:
  static std::string _opToString(Op v) noexcept {
    switch (v) {
    case ADDI:
      return "addi";
    case ANDI:
      return "andi";
    case SLLIW:
      return "slliw";
    case SRAIW:
      return "sraiw";
    case SRLI:
      return "srli";
    case SRLIW:
      return "srliw";
    case XORI:
      return "xori";
    case SLTI:
      return "slti";
    default:
      return "incomplete{_opToString}";
    }
  }
};

class RRR : public MIR {
public:
  enum Op {
    ADD,
    ADDW,
    SUB,
    SUBW,
    MUL,
    MULW,
    DIV,
    DIVW,
    REMW,
    EQ,
    GE,
    GT,
    LE,
    LT,
    AND,
    XOR,
    SLT,
    SGT
  };

  Op op;
  reg::Reg *dest, *src1, *src2;

  RRR(Op op, reg::Reg *dest, reg::Reg *src1, reg::Reg *src2)
      : op(op), dest(dest), src1(src1), src2(src2) {}

  std::vector<reg::Reg *> getRead() override { return {src1, src2}; }

  std::vector<reg::Reg *> getWrite() override { return {dest}; }

  MIR *replaceReg(
      const std::unordered_map<reg::Virtual *, reg::Machine *> &replaceMap) override {
    reg::Reg *newDest = dest, *newSrc1 = src1, *newSrc2 = src2;
    if (auto vdest = dynamic_cast<reg::Virtual *>(dest)) {
      if (replaceMap.find(vdest) != replaceMap.end())
        newDest = replaceMap.at(vdest);
    }
    if (auto vsrc1 = dynamic_cast<reg::Virtual *>(src1)) {
      if (replaceMap.find(vsrc1) != replaceMap.end())
        newSrc1 = replaceMap.at(vsrc1);
    }
    if (auto vsrc2 = dynamic_cast<reg::Virtual *>(src2)) {
      if (replaceMap.find(vsrc2) != replaceMap.end())
        newSrc2 = replaceMap.at(vsrc2);
    }
    return new RRR(op, newDest, newSrc1, newSrc2);
  }

  std::vector<MIR *> spill(reg::Reg *reg, int offset) override;
  [[nodiscard]] std::string str() const override;

private:
  static std::string _opToString(Op v) noexcept {
    switch (v) {
    case ADD:
      return "add";
    case ADDW:
      return "addw";
    case SUB:
      return "sub";
    case SUBW:
      return "subw";
    case MUL:
      return "mul";
    case MULW:
      return "mulw";
    case DIV:
      return "div";
    case DIVW:
      return "divw";
    case REMW:
      return "remw";
    case EQ:
      return "eq";
    case GE:
      return "ge";
    case GT:
      return "gt";
    case LE:
      return "le";
    case LT:
      return "lt";
    case AND:
      return "and";
    case XOR:
      return "xor";
    case SLT:
      return "slt";
    case SGT:
      return "sgt";
    default:
      return "incomplete{_opToString}";
    }
  }
};

class StoreTo : public MIR {
public:
  enum To { LOCAL, CALL_PARAM, INNER_PARAM, OUTER_PARAM, SPILL };

  To to;
  reg::Reg *src;
  int imm;

  StoreTo(To item, reg::Reg *src, int imm)
      : to(item), src(src), imm(imm) {}

  std::vector<reg::Reg *> getRead() override { return {src}; }

  MIR *replaceReg(
      const std::unordered_map<reg::Virtual *, reg::Machine *> &replaceMap) override {
    reg::Reg *newSrc = src;
    if (auto vsrc = dynamic_cast<reg::Virtual *>(src)) {
      if (replaceMap.find(vsrc) != replaceMap.end())
        newSrc = replaceMap.at(vsrc);
    }
    return new StoreTo(to, newSrc, imm);
  }

  std::vector<MIR *> spill(reg::Reg *reg, int offset) override;

  [[nodiscard]] std::string str() const override {
    return "store\t" + src->str() + ", " + std::to_string(imm) + "($" +
           _itemToString(to) + ")";
  }

private:
  static std::string _itemToString(To v) noexcept {
    switch (v) {
    case LOCAL:
      return "local";
    case CALL_PARAM:
      return "param_call";
    case INNER_PARAM:
      return "param_inner";
    case OUTER_PARAM:
      return "param_outer";
    case SPILL:
      return "spill";
    default:
      return "incomplete{_itemToString}";
    }
  }
};

class Store : public MIR {
public:
  reg::Reg *src, *dest;
  int imm, size;

  Store(reg::Reg *src, reg::Reg *dest, int imm, int size)
      : src(src), dest(dest), imm(imm), size(size) {}

  std::vector<reg::Reg *> getRead() override { return {src, dest}; }

  MIR *replaceReg(
      const std::unordered_map<reg::Virtual *, reg::Machine *> &replaceMap) override {
    reg::Reg *newSrc = src, *newDest = dest;
    if (auto vsrc = dynamic_cast<reg::Virtual *>(src)) {
      if (replaceMap.find(vsrc) != replaceMap.end())
        newSrc = replaceMap.at(vsrc);
    }
    if (auto vdest = dynamic_cast<reg::Virtual *>(dest)) {
      if (replaceMap.find(vdest) != replaceMap.end())
        newDest = replaceMap.at(vdest);
    }
    return new Store(newSrc, newDest, imm, size);
  }

  std::vector<MIR *> spill(reg::Reg *reg, int offset) override;
  [[nodiscard]] std::string str() const override;
};

} // namespace mir

#endif // MIR_MIR_HPP