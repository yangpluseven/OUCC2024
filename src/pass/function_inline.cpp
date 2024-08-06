#include "pass.hpp"

namespace pass {

std::vector<ir::BasicBlock *> getInline(ir::Function *from, ir::Function *to,
                                        std::vector<ir::Value *> &params) {
  std::unordered_map<ir::Value *, ir::Value *> replaceMap;
  std::vector<ir::BasicBlock *> newBlocks;
  const auto &args = from->getArgs();
  if (args.size() != params.size()) {
    return std::move(newBlocks);
  }
  // replace arguments with parameters
  for (int i = 0; i < args.size(); i++) {
    replaceMap[args[i]] = params[i];
  }
  // replace old labels with new labels
  for (const auto block : *from) {
    const auto newBlock = new ir::BasicBlock(to);
    newBlocks.push_back(newBlock);
    replaceMap[block] = newBlock;
  }
  // constants and global variables can share the same original value
  for (const auto block : *from) {
    for (const auto inst : *block) {
      for (const auto use : *inst) {
        if (dynamic_cast<ir::Constant *>(use->getValue()) ||
            dynamic_cast<ir::GlobalVariable *>(use->getValue())) {
          replaceMap[use->getValue()] = use->getValue();
        }
      }
    }
  }

  for (int i = 0; i < newBlocks.size(); i++) {
    const auto originBlock = from->get(i);
    const auto newBlock = newBlocks[i];
    for (const auto inst : *originBlock) {
    }
  }

  return std::move(newBlocks);
}

bool FunctionInline::onFunction(ir::Function *function) {}

} // namespace pass