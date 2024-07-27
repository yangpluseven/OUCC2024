#include "ir/function.hpp"
#include <algorithm>
#include <utility>

namespace ir {

Function::Function(Type *type, std::string name)
    : Value(type), name(std::move(name)) {}

bool Function::isDeclare() const { return blocks.empty(); }

Function *Function::addArg(Argument *arg) {
  args.push_back(arg);
  return this;
}

bool Function::add(BasicBlock *block) {
  blocks.push_back(block);
  return true;
}

void Function::add(int index, BasicBlock *block) {
  blocks.insert(blocks.begin() + index, block);
}

bool Function::add(int index, const std::vector<BasicBlock *> &newBlocks) {
  blocks.insert(blocks.begin() + index, newBlocks.begin(), newBlocks.end());
  return true;
}

BasicBlock *Function::remove(int index) {
  BasicBlock *removed = blocks[index];
  blocks.erase(blocks.begin() + index);
  return removed;
}

size_t Function::size() const { return blocks.size(); }

BasicBlock *Function::get(int index) const { return blocks[index]; }

BasicBlock *Function::getFirst() const { return blocks.front(); }

void Function::insertAfter(const BasicBlock *base, BasicBlock *block) {
  auto it = std::find(blocks.begin(), blocks.end(), base);
  if (it != blocks.end()) {
    blocks.insert(it + 1, block);
  }
}

std::vector<BasicBlock *>::iterator Function::begin() { return blocks.begin(); }

std::vector<BasicBlock *>::iterator Function::end() { return blocks.end(); }

std::vector<BasicBlock *>::const_iterator Function::begin() const {
  return blocks.begin();
}

std::vector<BasicBlock *>::const_iterator Function::end() const {
  return blocks.end();
}

std::vector<BasicBlock *>::const_iterator Function::cbegin() const {
  return blocks.cbegin();
}

std::vector<BasicBlock *>::const_iterator Function::cend() const {
  return blocks.cend();
}

std::string Function::getName() const { return "@" + name; }

std::string Function::getRawName() const { return name; }

std::vector<Argument *> Function::getArgs() const { return args; }

std::string Function::toString() const {
  bool const isDeclare = blocks.empty();
  std::ostringstream builder;
  std::ostringstream joiner;
  joiner << "(";
  for (size_t i = 0; i < args.size(); ++i) {
    if (i > 0)
      joiner << ", ";
    if (isDeclare)
      joiner << args[i]->getType()->toString();
    else
      joiner << args[i]->toString();
  }
  joiner << ")";
  if (isDeclare)
    builder << "declare ";
  else
    builder << "define ";
  builder << type->toString() << " " << getName() << joiner.str();
  if (isDeclare)
    return builder.str() + "\n";
  builder << " {\n";
  for (const auto &block : blocks) {
    builder << block->toString() << ":\n";
    for (const auto &inst : *block) {
      builder << "  " << inst->toString() << "\n";
    }
  }
  builder << "}\n";
  return builder.str();
}

Argument::Argument(Type *type, std::string name) : Value(type), name(std::move(name)) {}

std::string Argument::getName() const { return "%" + name; }

std::string Argument::toString() const { return type->toString(); }

} // namespace ir
