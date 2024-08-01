#include "preprocess.hpp"

const std::regex Preprocessor::ESCAPED_NEW_LINE_PATTERN("\\\\\\n");
const std::regex Preprocessor::COMMENT_PATTERN("//.*|/\\*[\\s\\S]*?\\*/");

void Preprocessor::replaceMacroFuncWithLineNo(const std::string &macro,
                                              const std::string &func) {
  std::regex macroPattern("\\b" + macro + "\\s*\\(\\s*\\)");
  std::string result;
  std::smatch matcher;
  std::string::const_iterator searchStart(content.cbegin());

  while (
      std::regex_search(searchStart, content.cend(), matcher, macroPattern)) {
    size_t start = matcher.position();
    size_t lineNo =
        std::count(content.begin(), content.begin() + start, '\n') + 1;
    result += content.substr(searchStart - content.cbegin(),
                             start - (searchStart - content.cbegin()));
    result += func + "(" + std::to_string(lineNo) + ")";
    searchStart += matcher.position() + matcher.length();
  }
  result += content.substr(searchStart - content.cbegin());
  content = result;
}
