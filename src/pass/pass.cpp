#include "pass.hpp"

namespace pass {

bool DCEPass::runOnFunction(ir::Function *function) {
  bool modified = false;
  for (const auto block : *function) {
    for (int i = 0; i + 1 < block->size(); i++) {
      const auto inst = block->get(i);
      const auto callInst = dynamic_cast<ir::CallInst *>(inst);
      const auto storeInst = dynamic_cast<ir::StoreInst *>(inst);
      if (!(callInst || storeInst) && inst->getUses().empty()) {
        block->remove(i);
        i--;
        modified = true;
      }
    }
  }
  return modified;
}

bool BranchOptPass::runOnFunction(ir::Function *function) { return false; }

bool ConstPropPass::runOnFunction(ir::Function *function) { return false; }

bool PromotePass::runOnFunction(ir::Function *function) { return false; }

} // namespace pass
