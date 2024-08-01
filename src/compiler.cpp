#include "compiler.hpp"
#include "ANTLRInputStream.h"
#include "SysYLexer.h"
#include "SysYParser.h"
#include "reg_alloc.hpp"

#include <fstream>
#include <sstream>

std::unordered_map<std::string, std::string> Compiler::optionValues{
    {"-S", "0"},     {"-O1", "0"},        {"-o", "out"},     {"", "main.sy"},
    {"-dispf", "0"}, {"-emit-llvm", "0"}, {"-emit-mir", "0"}};

ir::Module *Compiler::_module;
std::unordered_map<std::string, mir::MachineFunction *> Compiler::_funcs;

void Compiler::compile() {
  const std::string filePath = optionValues[""];
  std::ifstream file(filePath);
  if (!file.is_open()) {
    throw std::runtime_error("File not found");
  }
  std::stringstream buffer;
  buffer << file.rdbuf();
  antlr4::ANTLRInputStream input(buffer.str());
  SysYLexer lexer(&input);
  antlr4::CommonTokenStream tokens(&lexer);
  SysYParser parser(&tokens);
  SysYParser::RootContext *root = parser.root();
  parser::ASTVisitor visitor(root);
  _module = visitor.getModule();
  if (optionValues["-emit-llvm"] == "1") {
    emitLLVM();
    if (optionValues["-emit-mir"] == "1" || optionValues["-S"] == "1") {
      std::cout << "Emit llvm prior to mir and assembly. quitting."
                << std::endl;
      return;
    }
  }
  mir::MIRGenerator mirGenerator(_module);
  std::unordered_set<ir::GlobalVariable *> globals = mirGenerator.getGlobals();
  _funcs = mirGenerator.getFuncs();
  if (optionValues["-emit-mir"] == "1") {
    emitMIR();
    if (optionValues["-S"] == "1") {
      std::cout << "Emit mir prior to assembly. quitting." << std::endl;
      return;
    }
  }
  if (optionValues["-S"] == "1") {
    reg::ModuleRegAlloc regAlloc(_funcs);
    regAlloc.allocate();
    CodeGenerator codeGenerator(std::move(globals), std::move(_funcs));
    std::string output = codeGenerator.getOutPut();
    const std::string filename = optionValues["-o"];
    std::ofstream writer(filename);
    writer << output;
    writer.close();
    if (writer.fail()) {
      throw std::runtime_error("Cannot write to file: " + filename);
    }
  }
}

void Compiler::emitLLVM() {
  const std::string filename = optionValues["-o"];
  std::ofstream writer(filename);
  if (!writer.is_open()) {
    throw std::runtime_error("Cannot open file: " + filename);
  }

  for (const auto &global : _module->getGlobals()) {
    writer << global->toString() << '\n';
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
    writer << func->toString() << '\n';
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
    writer << func->toString() << '\n';
  }
  writer.close();
  if (writer.fail()) {
    throw std::runtime_error("Cannot write to file: " + filename);
  }
}

void CodeGenerator::buildFuncs(std::ostringstream &builder) const {
  builder << "\t.text\n";
  for (const auto &[key, func] : _funcs) {
    builder << "\t.align 8\n";
    builder << "\t.global " << func->getRawName() << '\n';
    builder << func->getRawName() << ":\n";
    for (const auto ir : func->getIRs()) {
      if (!dynamic_cast<const mir::LabelMIR *>(ir)) {
        builder << '\t';
      }
      std::string irStr = ir->toString();
      size_t pos = 0;
      while ((pos = irStr.find('\n', pos)) != std::string::npos) {
        irStr.insert(pos + 1, "\t");
        pos += 2;
      }
      builder << irStr << '\n';
    }
    builder << "\tret\n";
  }
}

void CodeGenerator::buildGlobals(std::ostringstream &builder) const {
  std::vector<ir::GlobalVariable *> symbolsInData;
  std::vector<ir::GlobalVariable *> symbolsInBss;
  for (const auto &global : _globals) {
    if (!global->isSingle() && global->isInBss()) {
      symbolsInBss.push_back(global);
    } else {
      symbolsInData.push_back(global);
    }
  }
  if (!symbolsInBss.empty()) {
    builder << "\t.bss\n";
  }
  for (const auto &global : symbolsInBss) {
    const int size = static_cast<int>(global->getSize()) / 8;
    builder << "\t.align 8\n";
    builder << "\t.size " << global->getRawName() << ", "
            << std::to_string(size) << '\n';
    builder << global->getRawName() << ":\n";
    builder << "\t.space " << std::to_string(size) << '\n';
  }
  if (!symbolsInData.empty()) {
    builder << "\t.data\n";
  }
  for (const auto &global : symbolsInData) {
    const int size = static_cast<int>(global->getSize()) / 8;
    builder << "\t.align 8\n";
    builder << "\t.size " << global->getRawName() << ", "
            << std::to_string(size) << '\n';
    builder << global->getRawName() << ":\n";
    const int num = size / 4;
    if (global->isSingle()) {
      builder << "\t.word ";
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
      builder << value << '\n';
    } else {
      auto type = global->getType();
      while (auto arrayType = dynamic_cast<const ir::ArrayType *>(type)) {
        type = arrayType->baseType();
      }
      for (int i = 0; i < num; i++) {
        builder << "\t.word ";
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
        builder << value << '\n';
      }
    }
  }
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

  return 0;
}