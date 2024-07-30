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
  int localSize = 0;
  std::unordered_map<ir::AllocaInst *, int> localOffsets;
  for (const auto ir : block) {
    auto const allocaInst = dynamic_cast<ir::AllocaInst *>(ir);
    if (!allocaInst) {
      break;
    }
    int size =
        static_cast<int>(allocaInst->getType()->baseType()->getSize() / 8);
    localOffsets[allocaInst] = localSize;
    localSize += size;
  }
  return {localSize, localOffsets};
}
