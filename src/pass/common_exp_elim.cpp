#include "pass.hpp"

namespace pass {

bool CommonExpElim::onFunction(ir::Function *function) {
  if (function->isDeclare()) {
    return false;
  }
  bool changed = false;
  std::vector expMap(
      10, std::unordered_map<unsigned int, ir::Instruction *>());
  std::unordered_map<std::string, ir::Instruction *> getPtrMap;

  std::vector<ir::Instruction *> toErase;
  for (auto *block : *function) {
    for (auto &map : expMap) {
      map.clear();
    }
    toErase.clear();
    getPtrMap.clear();

    for (auto *inst : *block) {

      if (const auto storeInst = dynamic_cast<ir::StoreInst *>(inst)) {
        auto *ptr = storeInst->getOperand<ir::Value>(1);
        auto hashCode = static_cast<unsigned int>(ptr->valueId);
        if (expMap[1].find(hashCode) != expMap[1].end()) {
          expMap[1].erase(hashCode);
        }
      }

      if (dynamic_cast<ir::GetPtrInst *>(inst)) {
        auto str = inst->baseStr();
        if (str.empty()) {
          continue;
        }
        if (getPtrMap.find(str) != getPtrMap.end() && getPtrMap.at(str) != nullptr) {
          inst->replaceAllUseAs(getPtrMap.at(str));
          toErase.push_back(inst);
          changed = true;
        } else {
          getPtrMap[str] = inst;
        }
        continue;
      }

      auto classId = inst->getClassId();
      if (classId == 0xFFFFFFFF) {
        continue;
      }
      auto hashCode = inst->hashCode();

      if (expMap[classId].find(hashCode) != expMap[classId].end()) {
        inst->replaceAllUseAs(expMap[classId].at(hashCode));
        toErase.push_back(inst);
        changed = true;
      } else {
        expMap[classId][hashCode] = inst;
      }
    }

    for (auto *inst : toErase) {
      block->erase(inst);
      inst->clear();
    }
  }
  return changed;
}

} // namespace pass