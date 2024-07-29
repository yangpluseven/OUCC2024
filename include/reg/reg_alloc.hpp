#ifndef REG_REG_ALLOC_HPP
#define REG_REG_ALLOC_HPP
#include "mir/mir_gen.hpp"

namespace reg {

class Block {
private:
  const int begin, end;
  std::unordered_set<Reg *> liveUse, liveDef;
  std::unordered_set<Reg *> liveIn, liveOut;
  std::unordered_set<Block *> nexts;

public:
  Block(int begin, int end) : begin(begin), end(end) {}

  void addUse(Reg *reg) { liveUse.insert(reg); }

  void addDef(Reg *reg) { liveDef.insert(reg); }

  void addNext(Block *next) { nexts.insert(next); }

  void calcIn() {
    liveIn.clear();
    liveIn.insert(liveOut.begin(), liveOut.end());
    for (const auto &def : liveDef) {
      liveIn.erase(def);
    }
    liveIn.insert(liveUse.begin(), liveUse.end());
  }

  void calcOut() {
    for (const auto &next : nexts) {
      liveOut.insert(next->liveIn.begin(), next->liveIn.end());
    }
  }

  [[nodiscard]] bool containsInDef(Reg *reg) const {
    return liveOut.find(reg) != liveOut.end();
  }

  [[nodiscard]] int getBegin() const { return begin; }

  [[nodiscard]] int getEnd() const { return end; }

  [[nodiscard]] std::unordered_set<Reg *> getRegs() const {
    std::unordered_set<Reg *> regs;
    regs.insert(liveUse.begin(), liveUse.end());
    regs.insert(liveDef.begin(), liveDef.end());
    regs.insert(liveIn.begin(), liveIn.end());
    regs.insert(liveOut.begin(), liveOut.end());
    return regs;
  }

  [[nodiscard]] std::unordered_set<Reg *> getOut() const { return liveOut; }

  [[nodiscard]] size_t sizeOfInOut() const {
    return liveIn.size() + liveOut.size();
  }
};

class FuncRegAlloc {
private:
  mir::MachineFunction *func;
  std::vector<MReg *> iCallerRegs;
  std::vector<MReg *> fCallerRegs;
  std::vector<MReg *> iCalleeRegs;
  std::vector<MReg *> fCalleeRegs;
  int funcParamSize{}, alignSize{}, spillSize{}, localSize{};
  int savedRegSize{}, paramInnerSize;

  std::vector<Block *> calcBlocks();
  // TODO better consider this return type
  std::unordered_map<Reg *, std::unordered_set<Reg *>> calcConflictMap();
  void calcInOut(std::vector<Block *> blocks);
  std::unordered_set<Reg *, std::unordered_set<int>> calcLifespans();
  void calcUseDef(std::vector<Block *> blocks);
  std::unordered_set<VReg *, MReg *> calcVRegToMReg();
  void makeFrameInfo();
  void popFrame();
  void pushFrame();
  void replaceFakeMIRs();
  void solveSpill();

public:
  explicit FuncRegAlloc(mir::MachineFunction *func) : func(func) {
    iCalleeRegs = std::vector<MReg *>(MReg::I_CALLEE_REGS.begin(),
                                      MReg::I_CALLEE_REGS.end());
    fCallerRegs = std::vector<MReg *>(MReg::F_CALLER_REGS.begin(),
                                      MReg::F_CALLER_REGS.end());
    paramInnerSize = (func->getICallerNum() + func->getFCallerNum()) * 8;
  }

  void allocate();
};

class ModuleRegAlloc {
private:
  std::vector<mir::MachineFunction *> funcs;

public:
  explicit ModuleRegAlloc(
      const std::unordered_map<std::string, mir::MachineFunction *>& funcs) {
    for (const auto &[name, func] : funcs) {
      this->funcs.push_back(func);
    }
  }

  void allocate() {
    for (const auto func : funcs) {
      reg::FuncRegAlloc(func).allocate();
    }
  }
};

} // namespace reg

#endif // REG_REG_ALLOC_HPP
