#include "pass.hpp"

namespace pass {

bool DeadCodeEli::onFunction(ir::Function *function) {
  bool changed = false;
  for (const auto block : *function) {
    for (int i = 0; i + 1 < block->size(); i++) {
      const auto inst = block->get(i);
      const auto callInst = dynamic_cast<ir::CallInst *>(inst);
      const auto storeInst = dynamic_cast<ir::StoreInst *>(inst);
      if (!(callInst || storeInst) && inst->getUses().empty()) {
        block->remove(i);
        i--;
        changed = true;
      }
    }
  }
  return changed;
}

}