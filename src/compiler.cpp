#include "compiler.hpp"
#include "ANTLRInputStream.h"
#include "SysYLexer.h"
#include "SysYParser.h"
#include "pass_manager.hpp"
#include "preprocess.hpp"
#include "reg_alloc.hpp"

#include <fstream>
#include <sstream>

std::unordered_map<std::string, std::string> Compiler::optionValues{
    {"-S", "0"},     {"-O1", "0"},        {"-o", "out"},     {"", "main.sy"},
    {"-dispf", "0"}, {"-emit-llvm", "0"}, {"-emit-mir", "0"}};

ir::Module *Compiler::_module;
std::unordered_map<std::string, mir::MachineFunction *> Compiler::_funcs;
std::unordered_set<ir::GlobalVariable *> Compiler::_globals;

void Compiler::compile() {
  const std::string filePath = optionValues[""];
  std::ifstream file(filePath);
  Preprocessor preprocessor(filePath);
  std::string in = preprocessor.preprocess();
  antlr4::ANTLRInputStream input(in);
  SysYLexer lexer(&input);
  antlr4::CommonTokenStream tokens(&lexer);
  SysYParser parser(&tokens);
  SysYParser::RootContext *root = parser.root();
  parser::ASTVisitor visitor(root);
  _module = visitor.getModule();
  if (optionValues["-O1"] == "1") {
    pass::PassManager passManager(_module);
    passManager.run();
  }
  if (optionValues["-emit-llvm"] == "1") {
    emitLLVM();
    if (optionValues["-emit-mir"] == "1" || optionValues["-S"] == "1") {
      std::cout << "Emit llvm prior to mir and assembly. quitting."
                << std::endl;
    }
    return;
  }
  mir::Generator mirGenerator(_module);
  _globals = mirGenerator.getGlobals();
  _funcs = mirGenerator.getFuncs();
  if (optionValues["-emit-mir"] == "1") {
    emitMIR();
    if (optionValues["-S"] == "1") {
      std::cout << "Emit mir prior to assembly. quitting." << std::endl;
    }
    return;
  }
  if (optionValues["-S"] == "1") {
    reg::ModuleRegAlloc regAlloc(_funcs);
    regAlloc.allocate();
    emitAssemble();
  }
}

void Compiler::emitLLVM() {
  const std::string filename = optionValues["-o"];
  std::ofstream writer(filename);
  if (!writer.is_open()) {
    throw std::runtime_error("Cannot open file: " + filename);
  }

  for (const auto &global : _module->getGlobals()) {
    writer << global->str() << '\n';
  }

  if (_module->hasGlobal()) {
    writer << '\n';
  }

  auto functions = _module->getFunctions();
  sort(functions.begin(), functions.end(),
       [](const ir::Function *func1, const ir::Function *func2) {
         if (func1->isDeclare() != func2->isDeclare()) {
           return func1->isDeclare() < func2->isDeclare();
         }
         return func1->getName() < func2->getName();
       });

  for (const auto &func : functions) {
    writer << func->str() << '\n';
  }

  writer.close();
  if (writer.fail()) {
    throw std::runtime_error("Cannot write to file: " + filename);
  }
}

void Compiler::emitMIR() {
  const std::string filename = optionValues["-o"];
  std::ofstream writer(filename);
  if (!writer.is_open()) {
    throw std::runtime_error("Cannot open file: " + filename);
  }
  for (const auto &p : _funcs) {
    const auto func = p.second;
    writer << func->str() << '\n';
  }
  writer.close();
  if (writer.fail()) {
    throw std::runtime_error("Cannot write to file: " + filename);
  }
}

void Compiler::emitAssemble() {
  writeGlobals();
  writeFuncs();
}


void Compiler::writeFuncs() {
  const std::string filename = optionValues["-o"];
  std::ofstream writer(filename, std::ios::app);
  writer << "\t.text\n";
  for (const auto &[key, func] : _funcs) {
    writer << "\t.align 8\n";
    writer << "\t.global " << func->getRawName() << '\n';
    writer << func->getRawName() << ":\n";
    for (const auto ir : func->getIRs()) {
      if (!dynamic_cast<const mir::Label *>(ir)) {
        writer << '\t';
      }
      std::string irStr = ir->str();
      size_t pos = 0;
      while ((pos = irStr.find('\n', pos)) != std::string::npos) {
        irStr.insert(pos + 1, "\t");
        pos += 2;
      }
      writer << irStr << '\n';
    }
    writer << "\tret\n";
  }
  writer.close();
  if (writer.fail()) {
    throw std::runtime_error("Cannot write to file: " + filename);
  }
}



void Compiler::writeGlobals() {
  std::vector<ir::GlobalVariable *> symbolsInData;
  std::vector<ir::GlobalVariable *> symbolsInBss;
  const std::string filename = optionValues["-o"];
  std::ofstream writer(filename);
  for (const auto &global : _globals) {
    if (!global->isSingle() && global->isInBss()) {
      symbolsInBss.push_back(global);
    } else {
      symbolsInData.push_back(global);
    }
  }
  if (!symbolsInBss.empty()) {
    writer << "\t.bss\n";
  }
  for (const auto &global : symbolsInBss) {
    const int size = static_cast<int>(global->getSize() / 8);
    writer << "\t.align 8\n";
    writer << "\t.size " << global->getRawName() << ", " << std::to_string(size)
           << '\n';
    writer << global->getRawName() << ":\n";
    writer << "\t.space " << std::to_string(size) << '\n';
  }
  if (!symbolsInData.empty()) {
    writer << "\t.data\n";
  }
  for (const auto &global : symbolsInData) {
    const int size = static_cast<int>(global->getSize()) / 8;
    writer << "\t.align 8\n";
    writer << "\t.size " << global->getRawName() << ", " << std::to_string(size)
           << '\n';
    writer << global->getRawName() << ":\n";
    const int num = size / 4;
    if (global->isSingle()) {
      writer << "\t.word ";
      std::string value;
      const auto type = global->getType();
      if (type == ir::BasicType::FLOAT) {
        float f = global->getFloat();
        value = std::to_string(*reinterpret_cast<int *>(&f));
      } else if (type == ir::BasicType::I32) {
        value = std::to_string(global->getInt());
      } else {
        throw std::runtime_error(
            "Unsupported type in CodeGenerator::buildGlobals");
      }
      writer << value << '\n';
    } else {
      auto type = global->getType();
      while (auto arrayType = dynamic_cast<const ir::ArrayType *>(type)) {
        type = arrayType->baseType();
      }
      for (int i = 0; i < num; i++) {
        writer << "\t.word ";
        std::string value;
        if (type == ir::BasicType::FLOAT) {
          float f = global->getFloat(i);
          value = std::to_string(*reinterpret_cast<int *>(&f));
        } else if (type == ir::BasicType::I32) {
          value = std::to_string(global->getInt(i));
        } else {
          throw std::runtime_error(
              "Unsupported type in CodeGenerator::buildGlobals");
        }
        writer << value << '\n';
      }
    }
  }
  writer.close();
  if (writer.fail()) {
    throw std::runtime_error("Cannot write to file: " + filename);
  }
}

void test() {
  auto *module = new ir::Module();
  auto *func = new ir::Function(ir::BasicType::I32, "main");
  auto *block0 = new ir::BasicBlock(func);
  auto *block1 = new ir::BasicBlock(func);
  ir::Instruction *alloca0 = new ir::AllocaInst(block0, ir::BasicType::I32);
  ir::Instruction *store0 = new ir::StoreInst(
      block0, new ir::ConstantNumber(model::Number(0)), alloca0);
  ir::Instruction *sotre1 = new ir::StoreInst(
      block0, new ir::ConstantNumber(model::Number(10)), alloca0);
  ir::Instruction *branch0 = new ir::BranchInst(block0, block1);
  block0->add(alloca0);
  block0->add(store0);
  block0->add(sotre1);
  block0->add(branch0);
  func->pushBlock(block0);
  ir::Instruction *load0 = new ir::LoadInst(block1, alloca0);
  ir::Instruction *ret0 = new ir::RetInst(block1, load0);
  block1->add(load0);
  block1->add(ret0);
  func->pushBlock(block1);
  module->addFunction(func);

  pass::PassManager passManager(module);
  passManager.run();

  Compiler::setModule(module);
  Compiler::emitLLVM();
}

int main(const int argc, char *argv[]) {
  for (int i = 1; i < argc; i++) {
    const std::string op = argv[i];
    if (op[0] != '-') {
      Compiler::optionValues[""] = op;
      continue;
    }
    if (Compiler::optionValues[op] == "0") {
      Compiler::optionValues[op] = "1";
      continue;
    }
    if (i == argc - 1) {
      if (Compiler::optionValues[op] == "0")
        Compiler::optionValues[op] = "1";
      continue;
    }
    const std::string value = argv[i + 1];
    if (value[0] == '-') {
      if (Compiler::optionValues[op] == "0") {
        Compiler::optionValues[op] = "1";
      } else {
        std::cout << "Error: option " << op << " requires a value" << std::endl;
      }
      continue;
    }
    Compiler::optionValues[op] = value;
    i++;
  }
  if (Compiler::optionValues["-dispf"] == "1") {
    for (auto &[key, value] : Compiler::optionValues) {
      std::cout << key << ": " << value << std::endl;
    }
  }

  Compiler::compile();
  // test();

  return 0;
}