#include "basic_block.hpp"
#include "type.hpp"

namespace ir {

int BasicBlock::_counter = 0;

BasicBlock::BasicBlock(Function *func)
    : Value(BasicType::VOID), _id(_counter++), _function(func) {}

bool BasicBlock::isEmpty() const { return _instructions.empty(); }

Instruction *BasicBlock::getLast() const {
  if (!_instructions.empty()) {
    return _instructions.back();
  } else {
    throw std::out_of_range("No _instructions in the BasicBlock");
  }
}

void BasicBlock::add(Instruction *inst) { _instructions.push_back(inst); }

void BasicBlock::add(int index, Instruction *inst) {
  if (index < 0 || index > _instructions.size()) {
    throw std::out_of_range("Index out of range");
  }
  _instructions.insert(_instructions.begin() + index, inst);
}

void BasicBlock::addAll(int index, const std::vector<Instruction *> &newInsts) {
  if (index < 0 || index > _instructions.size()) {
    throw std::out_of_range("Index out of range");
  }
  _instructions.insert(_instructions.begin() + index, newInsts.begin(),
                       newInsts.end());
}

Instruction *BasicBlock::remove(int index) {
  if (index < 0 || index >= _instructions.size()) {
    throw std::out_of_range("Index out of range");
  }
  Instruction *removedInst = _instructions[index];
  _instructions.erase(_instructions.begin() + index);
  return removedInst;
}

Instruction *BasicBlock::get(int index) const {
  if (index < 0 || index >= _instructions.size()) {
    throw std::out_of_range("Index out of range");
  }
  return _instructions[index];
}

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