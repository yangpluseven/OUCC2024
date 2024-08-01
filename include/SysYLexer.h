
// Generated from SysY.g4 by ANTLR 4.13.1

#pragma once


#include "antlr4-runtime.h"




class  SysYLexer : public antlr4::Lexer {
public:
  enum {
    BREAK = 1, CONST = 2, CONTINUE = 3, ELSE = 4, FLOAT = 5, IF = 6, INT = 7, 
    RETURN = 8, VOID = 9, WHILE = 10, ASSIGN = 11, ADD = 12, SUB = 13, MUL = 14, 
    DIV = 15, MOD = 16, EQ = 17, NE = 18, LT = 19, LE = 20, GT = 21, GE = 22, 
    LNOT = 23, LAND = 24, LOR = 25, LP = 26, RP = 27, LB = 28, RB = 29, 
    LC = 30, RC = 31, COMMA = 32, SEMI = 33, Ident = 34, IntConst = 35, 
    FloatConst = 36, Whitespace = 37, Newline = 38, BlockComment = 39, LineComment = 40
  };

  explicit SysYLexer(antlr4::CharStream *input);

  ~SysYLexer() override;


  std::string getGrammarFileName() const override;

  const std::vector<std::string>& getRuleNames() const override;

  const std::vector<std::string>& getChannelNames() const override;

  const std::vector<std::string>& getModeNames() const override;

  const antlr4::dfa::Vocabulary& getVocabulary() const override;

  antlr4::atn::SerializedATNView getSerializedATN() const override;

  const antlr4::atn::ATN& getATN() const override;

  // By default the static state used to implement the lexer is lazily initialized during the first
  // call to the constructor. You can call this function if you wish to initialize the static state
  // ahead of time.
  static void initialize();

private:

  // Individual action functions triggered by action() above.

  // Individual semantic predicate functions triggered by sempred() above.

};

