#ifndef MIR_MIR_HPP
#define MIR_MIR_HPP
#include "reg/register.hpp"

#include <unordered_map>

namespace mir {

class MIR {
private:
  static int _counter;
  int _id;

public:
  MIR() : _id(_counter++) {
  }

  virtual ~MIR() = default;

  virtual std::vector<reg::Reg *> getRead() {
    return {};
  }

  virtual std::vector<reg::Reg *> getWrite() {
    return {};
  }

  virtual MIR *replaceReg(
      const std::unordered_map<reg::VReg *, reg::MReg> &replaceMap) {
    return this;
  }

  virtual std::vector<MIR *> spill(reg::Reg *reg, int offset) {
    return {this};
  }

  std::vector<reg::Reg *> getRegs() {
    std::vector<reg::Reg *> regs;
    auto rd = getRead();
    auto wr = getWrite();
    regs.insert(regs.end(), rd.begin(), rd.end());
    regs.insert(regs.end(), wr.begin(), wr.end());
    return regs;
  }
};



}

#endif //MIR_MIR_HPP