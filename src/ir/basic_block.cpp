#include "basic_block.hpp"
#include "type.hpp"
#include <algorithm>

namespace ir {

int BasicBlock::_counter = 0;

BasicBlock::BasicBlock(Function *func)
    : Value(BasicType::VOID), _id(_counter++), _function(func) {}

bool BasicBlock::isEmpty() const { return _instructions.empty(); }

void BasicBlock::push(Instruction *inst) { _instructions.push_back(inst); }

void BasicBlock::insert(int index, Instruction *inst) {
  _instructions.insert(_instructions.begin() + index, inst);
}

void BasicBlock::insert(int index, const std::vector<Instruction *> &newInsts) {
  _instructions.insert(_instructions.begin() + index, newInsts.begin(),
                       newInsts.end());
}

void BasicBlock::insert(const Instruction *base, Instruction *inst) {
  auto it = std::find(_instructions.begin(), _instructions.end(), base);
  if (it != _instructions.end()) {
    _instructions.insert(it + 1, inst);
  }
}

void BasicBlock::insert(const Instruction *base,
                        const std::vector<Instruction *> &newInst) {
  auto it = std::find(_instructions.begin(), _instructions.end(), base);
  if (it != _instructions.end()) {
    _instructions.insert(it + 1, newInst.begin(), newInst.end());
  }
}

Instruction *BasicBlock::erase(int index) {
  Instruction *removedInst = _instructions[index];
  _instructions.erase(_instructions.begin() + index);
  return removedInst;
}

Instruction *BasicBlock::erase(Instruction *inst) {
  _instructions.erase(
      std::find(_instructions.begin(), _instructions.end(), inst));
  return inst;
}

Instruction *BasicBlock::pop() {
  if (_instructions.empty())
    return nullptr;
  Instruction *removedInst = _instructions.back();
  _instructions.pop_back();
  return removedInst;
}

Instruction *BasicBlock::get(int index) const { return _instructions[index]; }

size_t BasicBlock::size() const { return _instructions.size(); }

std::string BasicBlock::getName() const { return "b" + std::to_string(_id); }

std::string BasicBlock::str() const { return getName(); }

std::vector<Instruction *>::iterator BasicBlock::begin() {
  return _instructions.begin();
}

std::vector<Instruction *>::iterator BasicBlock::end() {
  return _instructions.end();
}

std::vector<Instruction *>::const_iterator BasicBlock::begin() const {
  return _instructions.begin();
}

std::vector<Instruction *>::const_iterator BasicBlock::end() const {
  return _instructions.end();
}

std::vector<Instruction *>::const_iterator BasicBlock::cbegin() const {
  return _instructions.cbegin();
}

std::vector<Instruction *>::const_iterator BasicBlock::cend() const {
  return _instructions.cend();
}

} // namespace ir