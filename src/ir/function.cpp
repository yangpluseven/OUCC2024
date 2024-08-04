#include "function.hpp"
#include <algorithm>
#include <utility>

namespace ir {

Function::Function(Type *type, std::string name)
    : Value(type), _name(std::move(name)) {}

bool Function::isDeclare() const { return _blocks.empty(); }

Function *Function::pushArg(Argument *arg) {
  _args.push_back(arg);
  return this;
}

bool Function::pushBlock(BasicBlock *block) {
  _blocks.push_back(block);
  return true;
}

void Function::insertBlock(int index, BasicBlock *block) {
  _blocks.insert(_blocks.begin() + index, block);
}

bool Function::insertBlock(int index, const std::vector<BasicBlock *> &newBlocks) {
  _blocks.insert(_blocks.begin() + index, newBlocks.begin(), newBlocks.end());
  return true;
}

BasicBlock *Function::erase(int index) {
  BasicBlock *removed = _blocks[index];
  _blocks.erase(_blocks.begin() + index);
  return removed;
}

size_t Function::size() const { return _blocks.size(); }

BasicBlock *Function::get(int index) const { return _blocks[index]; }

BasicBlock *Function::getFirst() const { return _blocks.front(); }

void Function::insertAfter(const BasicBlock *base, BasicBlock *block) {
  auto it = std::find(_blocks.begin(), _blocks.end(), base);
  if (it != _blocks.end()) {
    _blocks.insert(it + 1, block);
  }
}

std::vector<BasicBlock *>::iterator Function::begin() { return _blocks.begin(); }

std::vector<BasicBlock *>::iterator Function::end() { return _blocks.end(); }

std::vector<BasicBlock *>::const_iterator Function::begin() const {
  return _blocks.begin();
}

std::vector<BasicBlock *>::const_iterator Function::end() const {
  return _blocks.end();
}

std::vector<BasicBlock *>::const_iterator Function::cbegin() const {
  return _blocks.cbegin();
}

std::vector<BasicBlock *>::const_iterator Function::cend() const {
  return _blocks.cend();
}

std::string Function::getName() const { return "@" + _name; }

std::string Function::getRawName() const { return _name; }

std::vector<Argument *> &Function::getArgs() { return _args; }

std::string Function::str() const {
  bool const isDeclare = _blocks.empty();
  std::ostringstream builder;
  std::ostringstream joiner;
  joiner << "(";
  for (size_t i = 0; i < _args.size(); ++i) {
    if (i > 0)
      joiner << ", ";
    if (isDeclare)
      joiner << _args[i]->getType()->str();
    else
      joiner << _args[i]->str();
  }
  joiner << ")";
  if (isDeclare)
    builder << "declare ";
  else
    builder << "define ";
  builder << type->str() << " " << getName() << joiner.str();
  if (isDeclare)
    return builder.str() + "\n";
  builder << " {\n";
  for (const auto &block : _blocks) {
    builder << block->str() << ":\n";
    for (const auto &inst : *block) {
      builder << "  " << inst->str() << "\n";
    }
  }
  builder << "}\n";
  return builder.str();
}

Argument::Argument(Type *type, std::string name)
    : Value(type), name(std::move(name)) {}

std::string Argument::getName() const { return "%" + name; }

std::string Argument::str() const { return type->str() + " %" + name; }

} // namespace ir
