#ifndef PREPROCESS_HPP
#define PREPROCESS_HPP

#include <fstream>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>

class Preprocessor {
private:
  static const std::regex ESCAPED_NEW_LINE_PATTERN;
  static const std::regex COMMENT_PATTERN;
  std::string content;

  void replaceMacroDef() { replaceBuiltinMacroFuncWithLineNo(); }

  void replaceBuiltinMacroFuncWithLineNo() {
    replaceMacroFuncWithLineNo("starttime", "_sysy_starttime");
    replaceMacroFuncWithLineNo("stoptime", "_sysy_stoptime");
  }

  void replaceMacroFuncWithLineNo(const std::string &macro,
                                  const std::string &func);

  void removeEscapedNewLine() {
    content = std::regex_replace(content, ESCAPED_NEW_LINE_PATTERN, "");
  }

  void removeComment() {
    content = std::regex_replace(content, COMMENT_PATTERN, "");
  }

public:
  explicit Preprocessor(const std::string &inputPath) {
    std::ifstream file(inputPath);
    if (!file) {
      throw std::runtime_error("Failed to open file: " + inputPath);
    }
    std::ostringstream ss;
    ss << file.rdbuf();
    content = ss.str();
  }

  std::string preprocess() {
    replaceMacroDef();
    removeEscapedNewLine();
    removeComment();
    return content;
  }
};
#endif // PREPROCESS_HPP
