#include "ir/basic_block.hpp"

namespace ir {

int BasicBlock::_counter = 0;

BasicBlock::BasicBlock(Function *func)
    : Value(BasicType::VOID), _function(func), _id(_counter++) {}

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

int BasicBlock::size() { return _instructions.size(); }

std::string BasicBlock::getName() const { return "b" + std::to_string(_id); }

std::string BasicBlock::toString() const { return getName(); }

BasicBlock::Iterator::Iterator(typename std::vector<Instruction *>::iterator it)
    : it(it) {}

Instruction *BasicBlock::Iterator::operator*() const { return *it; }

BasicBlock::Iterator &BasicBlock::Iterator::operator++() {
  ++it;
  return *this;
}

BasicBlock::Iterator BasicBlock::Iterator::operator++(int) {
  Iterator tmp = *this;
  ++it;
  return tmp;
}

bool BasicBlock::Iterator::operator==(const Iterator &rhs) const {
  return it == rhs.it;
}

bool BasicBlock::Iterator::operator!=(const Iterator &rhs) const {
  return it != rhs.it;
}

BasicBlock::Iterator BasicBlock::begin() {
  return Iterator(_instructions.begin());
}

BasicBlock::Iterator BasicBlock::end() { return Iterator(_instructions.end()); }

BasicBlock::ConstIterator::ConstIterator(
    typename std::vector<Instruction *>::const_iterator it)
    : it(it) {}

const Instruction *BasicBlock::ConstIterator::operator*() const { return *it; }

BasicBlock::ConstIterator &BasicBlock::ConstIterator::operator++() {
  ++it;
  return *this;
}

BasicBlock::ConstIterator BasicBlock::ConstIterator::operator++(int) {
  ConstIterator tmp = *this;
  ++it;
  return tmp;
}

bool BasicBlock::ConstIterator::operator==(const ConstIterator &other) const {
  return it == other.it;
}

bool BasicBlock::ConstIterator::operator!=(const ConstIterator &other) const {
  return it != other.it;
}

BasicBlock::ConstIterator BasicBlock::begin() const {
  return ConstIterator(_instructions.begin());
}

BasicBlock::ConstIterator BasicBlock::end() const {
  return ConstIterator(_instructions.end());
}

} // namespace ir
