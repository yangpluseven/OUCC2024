#include "pass.hpp"

namespace pass {

bool CommonExpElim::onFunction(ir::Function *function) {
  if (function->isDeclare()) {
    return false;
  }
  bool changed = false;
  std::unordered_map<std::string, ir::Instruction *> expMap;
  std::vector<ir::Instruction *> toErase;
  for (auto *block : *function) {
    expMap.clear();
    toErase.clear();
    for (auto *inst : *block) {

      // if (dynamic_cast<ir::LoadInst *>(inst)) {
      //   continue;
      // }

      if (const auto storeInst = dynamic_cast<ir::StoreInst *>(inst)) {
        auto *ptr = storeInst->getOperand<ir::Value>(1);
        ir::LoadInst tmpLoad(block, ptr);
        auto str = tmpLoad.baseStr();
        if (expMap.find(str) != expMap.end()) {
          expMap.erase(str);
        }
        // const auto &uses = ptr->getUses();
        // for (const auto use : uses) {
        //   if (const auto *loadInst = dynamic_cast<ir::LoadInst *>(use->getUser())) {
        //     auto str = loadInst->baseStr();
        //     if (expMap.find(str) != expMap.end()) {
        //       expMap.erase(str);
        //       // TODO can break?
        //       break;
        //     }
        //   }
        // }
      }

      auto str = inst->baseStr();
      if (str.empty()) {
        continue;
      }
      if (expMap.find(str) != expMap.end() && expMap.at(str) != nullptr) {
        inst->replaceAllUseAs(expMap.at(str));
        toErase.push_back(inst);
        changed = true;
      } else {
        expMap[str] = inst;
      }
    }
    for (auto *inst : toErase) {
      block->erase(inst);
    }
  }
  return changed;
}

} // namespace pass