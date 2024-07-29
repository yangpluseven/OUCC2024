#include "mir/mir_gen.hpp"

std::unordered_map<ir::Argument *, std::pair<bool, int>>
mir::MIRGenerator::calcArgOffsets(std::vector<ir::Argument *> &args) {
  // TODO
}

mir::MachineFunction *mir::MIRGenerator::funcToMir(ir::Function *func) {
  // TODO
}

std::pair<int, std::unordered_map<ir::AllocaInst *, int>>
mir::MIRGenerator::calcLocalOffsets(ir::BasicBlock *block) {
  // TODO
}
