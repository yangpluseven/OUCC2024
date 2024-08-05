#ifndef REG_REG_ALLOC_HPP
#define REG_REG_ALLOC_HPP

#include "mir_gen.hpp"

namespace reg {

class Block {
private:
  const int _begin, _end;
  std::unordered_set<Reg *> _liveUse, _liveDef;
  std::unordered_set<Reg *> _liveIn, _liveOut;
  std::unordered_set<Block *> _nexts;

public:
  Block(int begin, int end) : _begin(begin), _end(end) {}

  void addUse(Reg *reg) { _liveUse.insert(reg); }

  void addDef(Reg *reg) { _liveDef.insert(reg); }

  void addNext(Block *next) { _nexts.insert(next); }

  void calcIn() {
    _liveIn.clear();
    _liveIn.insert(_liveOut.begin(), _liveOut.end());
    for (const auto def : _liveDef) {
      if (_liveIn.find(def) != _liveIn.end()) {
        _liveIn.erase(def);
      }
    }
    _liveIn.insert(_liveUse.begin(), _liveUse.end());
  }

  void calcOut() {
    for (const auto next : _nexts) {
      _liveOut.insert(next->_liveIn.begin(), next->_liveIn.end());
    }
  }

  [[nodiscard]] bool containsInDef(Reg *reg) const {
    return _liveDef.find(reg) != _liveDef.end();
  }

  [[nodiscard]] int getBegin() const { return _begin; }

  [[nodiscard]] int getEnd() const { return _end; }

  [[nodiscard]] std::unordered_set<Reg *> getRegs() const {
    std::unordered_set<Reg *> regs;
    regs.insert(_liveUse.begin(), _liveUse.end());
    regs.insert(_liveDef.begin(), _liveDef.end());
    regs.insert(_liveIn.begin(), _liveIn.end());
    regs.insert(_liveOut.begin(), _liveOut.end());
    return std::move(regs);
  }

  [[nodiscard]] std::unordered_set<Reg *> &getOut() { return _liveOut; }

  [[nodiscard]] size_t sizeOfInOut() const {
    return _liveIn.size() + _liveOut.size();
  }
};

class FuncRegAlloc {
private:
  mir::MachineFunction *_func;
  std::vector<Machine *> _iCallerRegs;
  std::vector<Machine *> _fCallerRegs;
  std::vector<Machine *> _iCalleeRegs{};
  std::vector<Machine *> _fCalleeRegs{};
  int _funcParamSize{}, _alignSize{}, _spillSize{}, _localSize{};
  int _savedRegSize{}, _callAddrSize{}, _paramInnerSize;

  static void calcInOut(std::vector<Block *> &blocks);

  [[nodiscard]] std::vector<Block *> calcBlocks() const;
  std::unordered_map<Reg *, std::unordered_set<Reg *>> calcConflictMap();
  std::unordered_map<Reg *, std::unordered_set<int>> calcLifespans();
  void calcUseDef(std::vector<Block *> &blocks) const;
  std::unordered_map<Virtual *, Machine *> calcVRegToMReg();
  void makeFrameInfo();
  void popFrame();
  void pushFrame();
  void replaceFakeMIRs();
  void solveSpill();

public:
  explicit FuncRegAlloc(mir::MachineFunction *func) : _func(func) {
    _iCallerRegs = std::vector<Machine *>(Machine::I_CALLER_REGS.begin(),
                                       Machine::I_CALLER_REGS.begin() +
                                           _func->getICallerNum());
    _fCallerRegs = std::vector<Machine *>(Machine::F_CALLER_REGS.begin(),
                                       Machine::F_CALLER_REGS.begin() +
                                           _func->getFCallerNum());
    _paramInnerSize = (func->getICallerNum() + func->getFCallerNum()) * 8;
  }

  void allocate() {
    solveSpill();
    std::unordered_map<Virtual *, Machine *> vRegToMReg = calcVRegToMReg();
    auto &vec = _func->getMIRs();
    // TODO check here
    for (auto &i : vec) {
      if (!i) {
        continue;
      }
      i = i->replaceReg(vRegToMReg);
    }
    makeFrameInfo();
    pushFrame();
    popFrame();
    replaceFakeMIRs();
  }
};

class ModuleRegAlloc {
private:
  std::vector<mir::MachineFunction *> _funcs;

public:
  explicit ModuleRegAlloc(
      const std::unordered_map<std::string, mir::MachineFunction *> &funcs) {
    for (const auto &[name, func] : funcs) {
      this->_funcs.push_back(func);
    }
  }

  void allocate() {
    for (const auto func : _funcs) {
      FuncRegAlloc(func).allocate();
    }
  }
};

} // namespace reg

#endif // REG_REG_ALLOC_HPP
