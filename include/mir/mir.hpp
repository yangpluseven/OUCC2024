#ifndef MIR_MIR_HPP
#define MIR_MIR_HPP
#include "ir/basic_block.hpp"
#include "ir/global_variable.hpp"
#include "reg/register.hpp"

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
  replaceReg(const std::unordered_map<reg::VReg *, reg::MReg *> &replaceMap) {
    return this;
  }

  virtual std::vector<MIR *> spill(reg::Reg *reg, int offset) { return {this}; }

  [[nodiscard]] virtual std::string toString() const = 0;

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

class AddRegLocalMIR : public MIR {
public:
  reg::Reg *dest;
  int imm;

  AddRegLocalMIR(reg::Reg *dest, int imm) : dest(dest), imm(imm) {}

  std::vector<reg::Reg *> getWrite() override { return {dest}; }

  MIR *replaceReg(
      const std::unordered_map<reg::VReg *, reg::MReg *> &replaceMap) override {
    reg::Reg *newDest = dest;
    if (auto vdest = dynamic_cast<reg::VReg *>(dest)) {
      if (replaceMap.find(vdest) != replaceMap.end())
        newDest = replaceMap.at(vdest);
    }
    return new AddRegLocalMIR(newDest, imm);
  }

  std::vector<MIR *> spill(reg::Reg *reg, int offset) override;

  [[nodiscard]] std::string toString() const override {
    return "add\t" + dest->toString() + ", $local, #" + std::to_string(imm);
  }
};

class BMIR : public MIR {
public:
  enum Op { NUL, EQ, NE, LT, LE, GT, GE };

  Op op = NUL;
  reg::Reg *src1, *src2;
  ir::BasicBlock *block;

  BMIR(Op op, reg::Reg *src1, reg::Reg *src2, ir::BasicBlock *block)
      : op(op), src1(src1), src2(src2), block(block) {}

  std::vector<reg::Reg *> getRead() override {
    if (op == NUL) {
      return {};
    }
    return {src1, src2};
  }

  MIR *replaceReg(
      const std::unordered_map<reg::VReg *, reg::MReg *> &replaceMap) override {
    reg::Reg *newSrc1 = src1, *newSrc2 = src2;
    if (auto vsrc1 = dynamic_cast<reg::VReg *>(src1)) {
      if (replaceMap.find(vsrc1) != replaceMap.end())
        newSrc1 = replaceMap.at(vsrc1);
    }
    if (auto vsrc2 = dynamic_cast<reg::VReg *>(src2)) {
      if (replaceMap.find(vsrc2) != replaceMap.end())
        newSrc2 = replaceMap.at(vsrc2);
    }
    return new BMIR(op, newSrc1, newSrc2, block);
  }

  std::vector<MIR *> spill(reg::Reg *reg, int offset) override;

  [[nodiscard]] bool hasCond() const { return op != NUL; }

  [[nodiscard]] std::string toString() const override {
    if (op == NUL) {
      return "j\t" + block->getName();
    }
    return "b" + _opToString(op) + "\t" + src1->toString() + ", " +
           src2->toString() + ", " + block->getName();
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

class CallMIR : public MIR {
public:
  ir::Function *func;

  explicit CallMIR(ir::Function *func) : func(func) {}

  std::vector<reg::Reg *> getRead() override {
    std::vector<reg::Reg *> regs;
    int isize = 0, fsize = 0;
    for (auto arg : func->getArgs()) {
      if (arg->getType() == ir::BasicType::FLOAT) {
        if (fsize < reg::MReg::F_CALLER_REGS.size()) {
          regs.push_back(reg::MReg::F_CALLER_REGS.at(fsize));
        }
        fsize++;
      } else {
        if (isize < reg::MReg::I_CALLER_REGS.size()) {
          regs.push_back(reg::MReg::I_CALLER_REGS.at(isize));
        }
        isize++;
      }
    }
    return regs;
  };

  std::vector<reg::Reg *> getWrite() override {
    std::vector<reg::Reg *> regs;
    regs.insert(regs.end(), reg::MReg::I_CALLER_REGS.begin(),
                reg::MReg::I_CALLER_REGS.end());
    regs.insert(regs.end(), reg::MReg::F_CALLER_REGS.begin(),
                reg::MReg::F_CALLER_REGS.end());
    return regs;
  }

  [[nodiscard]] std::string toString() const override {
    return "call\t" + func->getRawName();
  }
};

class LabelMIR : public MIR {
public:
  ir::BasicBlock *block;

  explicit LabelMIR(ir::BasicBlock *block) : block(block) {}

  [[nodiscard]] ir::BasicBlock *getBlock() const { return block; }

  [[nodiscard]] std::string toString() const override {
    return block->getName() + ":";
  }
};

class LiMIR : public MIR {
public:
  reg::Reg *dest;
  int imm;

  LiMIR(reg::Reg *dest, int imm) : dest(dest), imm(imm) {}

  std::vector<reg::Reg *> getWrite() override { return {dest}; }

  MIR *replaceReg(
      const std::unordered_map<reg::VReg *, reg::MReg *> &replaceMap) override {
    reg::Reg *newDest = dest;
    if (auto vdest = dynamic_cast<reg::VReg *>(dest)) {
      if (replaceMap.find(vdest) != replaceMap.end())
        newDest = replaceMap.at(vdest);
    }
    return new LiMIR(newDest, imm);
  }

  std::vector<MIR *> spill(reg::Reg *reg, int offset) override;

  [[nodiscard]] std::string toString() const override {
    return "li\t" + dest->toString() + ", " + std::to_string(imm);
  }
};

class LlaMIR : public MIR {
public:
  reg::Reg *dest;
  ir::GlobalVariable *global;

  LlaMIR(reg::Reg *dest, ir::GlobalVariable *global)
      : dest(dest), global(global) {}

  std::vector<reg::Reg *> getWrite() override { return {dest}; }

  MIR *replaceReg(
      const std::unordered_map<reg::VReg *, reg::MReg *> &replaceMap) override {
    reg::Reg *newDest = dest;
    if (auto vdest = dynamic_cast<reg::VReg *>(dest)) {
      if (replaceMap.find(vdest) != replaceMap.end())
        newDest = replaceMap.at(vdest);
    }
    return new LlaMIR(newDest, global);
  }

  std::vector<MIR *> spill(reg::Reg *reg, int offset) override;

  [[nodiscard]] std::string toString() const override {
    return "lla\t" + dest->toString() + ", " + global->getRawName();
  }
};

class LoadItemMIR : public MIR {
public:
  enum Item { SPILL, PARAM_INNER, PARAM_OUTER, LOCAL };

  Item item;
  reg::Reg *dest;
  int imm;

  LoadItemMIR(Item item, reg::Reg *dest, int imm)
      : item(item), dest(dest), imm(imm) {}

  std::vector<reg::Reg *> getWrite() override { return {dest}; }

  MIR *replaceReg(
      const std::unordered_map<reg::VReg *, reg::MReg *> &replaceMap) override {
    reg::Reg *newDest = dest;
    if (auto vdest = dynamic_cast<reg::VReg *>(dest)) {
      if (replaceMap.find(vdest) != replaceMap.end()) {
        newDest = replaceMap.at(vdest);
      }
    }
    return new LoadItemMIR(item, newDest, imm);
  }

  std::vector<MIR *> spill(reg::Reg *reg, int offset) override;

  [[nodiscard]] std::string toString() const override {
    return "load\t" + dest->toString() + ", " + std::to_string(imm) + "($" +
           _itemToString(item) + ")";
  }

private:
  static std::string _itemToString(Item v) noexcept {
    switch (v) {
    case SPILL:
      return "spill";
    case PARAM_INNER:
      return "param_inner";
    case PARAM_OUTER:
      return "param_outer";
    case LOCAL:
      return "local";
    default:
      return "incomplete{_itemToString}";
    }
  }
};

class LoadMIR : public MIR {
public:
  reg::Reg *dest, *src;
  int imm, size;

  LoadMIR(reg::Reg *dest, reg::Reg *src, int imm, int size)
      : dest(dest), src(src), imm(imm), size(size) {}

  std::vector<reg::Reg *> getRead() override { return {src}; }

  std::vector<reg::Reg *> getWrite() override { return {dest}; }

  MIR *replaceReg(
      const std::unordered_map<reg::VReg *, reg::MReg *> &replaceMap) override {
    reg::Reg *newDest = dest, *newSrc = src;
    if (auto vdest = dynamic_cast<reg::VReg *>(dest)) {
      if (replaceMap.find(vdest) != replaceMap.end())
        newDest = replaceMap.at(vdest);
    }
    if (auto vsrc = dynamic_cast<reg::VReg *>(src)) {
      if (replaceMap.find(vsrc) != replaceMap.end())
        newSrc = replaceMap.at(vsrc);
    }
    return new LoadMIR(newDest, newSrc, imm, size);
  }

  std::vector<MIR *> spill(reg::Reg *reg, int offset) override;
  [[nodiscard]] std::string toString() const override;
};

class RrMIR : public MIR {
public:
  enum Op { CVT, FABS, MV, NEG, SEQZ, SNEZ };

  Op op;
  reg::Reg *dest, *src;

  RrMIR(Op op, reg::Reg *dest, reg::Reg *src) : op(op), dest(dest), src(src) {}

  std::vector<reg::Reg *> getRead() override { return {src}; }

  std::vector<reg::Reg *> getWrite() override { return {dest}; }

  MIR *replaceReg(
      const std::unordered_map<reg::VReg *, reg::MReg *> &replaceMap) override {
    reg::Reg *newDest = dest, *newSrc = src;
    if (auto vdest = dynamic_cast<reg::VReg *>(dest)) {
      if (replaceMap.find(vdest) != replaceMap.end())
        newDest = replaceMap.at(vdest);
    }
    if (auto vsrc = dynamic_cast<reg::VReg *>(src)) {
      if (replaceMap.find(vsrc) != replaceMap.end())
        newSrc = replaceMap.at(vsrc);
    }
    return new RrMIR(op, newDest, newSrc);
  }

  std::vector<MIR *> spill(reg::Reg *reg, int offset) override;
  [[nodiscard]] std::string toString() const override;

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

class RriMIR : public MIR {
public:
  enum Op { ADDI, ANDI, SLLIW, SRAIW, SRLI, SRLIW, XORI };

  Op op;
  reg::Reg *dest, *src;
  int imm;

  RriMIR(Op op, reg::Reg *dest, reg::Reg *src, int imm)
      : op(op), dest(dest), src(src), imm(imm) {}

  std::vector<reg::Reg *> getRead() override { return {src}; }

  std::vector<reg::Reg *> getWrite() override { return {dest}; }

  MIR *replaceReg(
      const std::unordered_map<reg::VReg *, reg::MReg *> &replaceMap) override {
    reg::Reg *newDest = dest, *newSrc = src;
    if (auto vdest = dynamic_cast<reg::VReg *>(dest)) {
      if (replaceMap.find(vdest) != replaceMap.end())
        newDest = replaceMap.at(vdest);
    }
    if (auto vsrc = dynamic_cast<reg::VReg *>(src)) {
      if (replaceMap.find(vsrc) != replaceMap.end())
        newSrc = replaceMap.at(vsrc);
    }
    return new RriMIR(op, newDest, newSrc, imm);
  }

  std::vector<MIR *> spill(reg::Reg *reg, int offset) override;

  [[nodiscard]] std::string toString() const override {
    return _opToString(op) + "\t" + dest->toString() + ", " + src->toString() +
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
    default:
      return "incomplete{_opToString}";
    }
  }
};

class RrrMIR : public MIR {
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

  RrrMIR(Op op, reg::Reg *dest, reg::Reg *src1, reg::Reg *src2)
      : op(op), dest(dest), src1(src1), src2(src2) {}

  std::vector<reg::Reg *> getRead() override { return {src1, src2}; }

  std::vector<reg::Reg *> getWrite() override { return {dest}; }

  MIR *replaceReg(
      const std::unordered_map<reg::VReg *, reg::MReg *> &replaceMap) override {
    reg::Reg *newDest = dest, *newSrc1 = src1, *newSrc2 = src2;
    if (auto vdest = dynamic_cast<reg::VReg *>(dest)) {
      if (replaceMap.find(vdest) != replaceMap.end())
        newDest = replaceMap.at(vdest);
    }
    if (auto vsrc1 = dynamic_cast<reg::VReg *>(src1)) {
      if (replaceMap.find(vsrc1) != replaceMap.end())
        newSrc1 = replaceMap.at(vsrc1);
    }
    if (auto vsrc2 = dynamic_cast<reg::VReg *>(src2)) {
      if (replaceMap.find(vsrc2) != replaceMap.end())
        newSrc2 = replaceMap.at(vsrc2);
    }
    return new RrrMIR(op, newDest, newSrc1, newSrc2);
  }

  std::vector<MIR *> spill(reg::Reg *reg, int offset) override;
  [[nodiscard]] std::string toString() const override;

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

class StoreItemMIR : public MIR {
public:
  enum Item { LOCAL, PARAM_CALL, PARAM_INNER, PARAM_OUTER, SPILL };

  Item item;
  reg::Reg *src;
  int imm;

  StoreItemMIR(Item item, reg::Reg *src, int imm)
      : item(item), src(src), imm(imm) {}

  std::vector<reg::Reg *> getRead() override { return {src}; }

  MIR *replaceReg(
      const std::unordered_map<reg::VReg *, reg::MReg *> &replaceMap) override {
    reg::Reg *newSrc = src;
    if (auto vsrc = dynamic_cast<reg::VReg *>(src)) {
      if (replaceMap.find(vsrc) != replaceMap.end())
        newSrc = replaceMap.at(vsrc);
    }
    return new StoreItemMIR(item, newSrc, imm);
  }

  std::vector<MIR *> spill(reg::Reg *reg, int offset) override;

  [[nodiscard]] std::string toString() const override {
    return "store\t" + src->toString() + ", " + std::to_string(imm) + "($" +
           _itemToString(item) + ")";
  }

private:
  static std::string _itemToString(Item v) noexcept {
    switch (v) {
    case LOCAL:
      return "local";
    case PARAM_CALL:
      return "param_call";
    case PARAM_INNER:
      return "param_inner";
    case PARAM_OUTER:
      return "param_outer";
    case SPILL:
      return "spill";
    default:
      return "incomplete{_itemToString}";
    }
  }
};

class StoreMIR : public MIR {
public:
  reg::Reg *src, *dest;
  int imm, size;

  StoreMIR(reg::Reg *src, reg::Reg *dest, int imm, int size)
      : src(src), dest(dest), imm(imm), size(size) {}

  std::vector<reg::Reg *> getRead() override { return {src, dest}; }

  MIR *replaceReg(
      const std::unordered_map<reg::VReg *, reg::MReg *> &replaceMap) override {
    reg::Reg *newSrc = src, *newDest = dest;
    if (auto vsrc = dynamic_cast<reg::VReg *>(src)) {
      if (replaceMap.find(vsrc) != replaceMap.end())
        newSrc = replaceMap.at(vsrc);
    }
    if (auto vdest = dynamic_cast<reg::VReg *>(dest)) {
      if (replaceMap.find(vdest) != replaceMap.end())
        newDest = replaceMap.at(vdest);
    }
    return new StoreMIR(newSrc, newDest, imm, size);
  }

  std::vector<MIR *> spill(reg::Reg *reg, int offset) override;
  [[nodiscard]] std::string toString() const override;
};

} // namespace mir

#endif // MIR_MIR_HPP