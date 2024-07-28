#include "../include/compiler.hpp"
#include <bits/stdc++.h>

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