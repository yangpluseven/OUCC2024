
// Generated from SysY.g4 by ANTLR 4.13.1


#include "parser/SysYVisitor.h"

#include "parser/SysYParser.h"


using namespace antlrcpp;

using namespace antlr4;

namespace {

struct SysYParserStaticData final {
  SysYParserStaticData(std::vector<std::string> ruleNames,
                        std::vector<std::string> literalNames,
                        std::vector<std::string> symbolicNames)
      : ruleNames(std::move(ruleNames)), literalNames(std::move(literalNames)),
        symbolicNames(std::move(symbolicNames)),
        vocabulary(this->literalNames, this->symbolicNames) {}

  SysYParserStaticData(const SysYParserStaticData&) = delete;
  SysYParserStaticData(SysYParserStaticData&&) = delete;
  SysYParserStaticData& operator=(const SysYParserStaticData&) = delete;
  SysYParserStaticData& operator=(SysYParserStaticData&&) = delete;

  std::vector<antlr4::dfa::DFA> decisionToDFA;
  antlr4::atn::PredictionContextCache sharedContextCache;
  const std::vector<std::string> ruleNames;
  const std::vector<std::string> literalNames;
  const std::vector<std::string> symbolicNames;
  const antlr4::dfa::Vocabulary vocabulary;
  antlr4::atn::SerializedATNView serializedATN;
  std::unique_ptr<antlr4::atn::ATN> atn;
};

::antlr4::internal::OnceFlag sysyParserOnceFlag;
#if ANTLR4_USE_THREAD_LOCAL_CACHE
static thread_local
#endif
SysYParserStaticData *sysyParserStaticData = nullptr;

void sysyParserInitialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  if (sysyParserStaticData != nullptr) {
    return;
  }
#else
  assert(sysyParserStaticData == nullptr);
#endif
  auto staticData = std::make_unique<SysYParserStaticData>(
    std::vector<std::string>{
      "root", "compUnit", "type", "dimensions", "varDecl", "varDef", "scalarVarDef", 
      "arrayVarDef", "initVal", "funcDef", "funcArg", "blockStmt", "stmt", 
      "assignStmt", "blankStmt", "expStmt", "ifElseStmt", "ifStmt", "whileStmt", 
      "breakStmt", "continueStmt", "retStmt", "lVal", "unaryExp", "varExp", 
      "scalarVarExp", "arrayVarExp", "funcCallExp", "numberExp", "lorExp", 
      "landExp", "equalityExp", "relationalExp", "additiveExp", "multiplicativeExp"
    },
    std::vector<std::string>{
      "", "'break'", "'const'", "'continue'", "'else'", "'float'", "'if'", 
      "'int'", "'return'", "'void'", "'while'", "'='", "'+'", "'-'", "'*'", 
      "'/'", "'%'", "'=='", "'!='", "'<'", "'<='", "'>'", "'>='", "'!'", 
      "'&&'", "'||'", "'('", "')'", "'['", "']'", "'{'", "'}'", "','", "';'", 
      "", "", "", "", "'\\n'"
    },
    std::vector<std::string>{
      "", "BREAK", "CONST", "CONTINUE", "ELSE", "FLOAT", "IF", "INT", "RETURN", 
      "VOID", "WHILE", "ASSIGN", "ADD", "SUB", "MUL", "DIV", "MOD", "EQ", 
      "NE", "LT", "LE", "GT", "GE", "LNOT", "LAND", "LOR", "LP", "RP", "LB", 
      "RB", "LC", "RC", "COMMA", "SEMI", "Ident", "IntConst", "FloatConst", 
      "Whitespace", "Newline", "BlockComment", "LineComment"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,1,40,330,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,6,2,
  	7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,2,13,7,13,2,14,7,
  	14,2,15,7,15,2,16,7,16,2,17,7,17,2,18,7,18,2,19,7,19,2,20,7,20,2,21,7,
  	21,2,22,7,22,2,23,7,23,2,24,7,24,2,25,7,25,2,26,7,26,2,27,7,27,2,28,7,
  	28,2,29,7,29,2,30,7,30,2,31,7,31,2,32,7,32,2,33,7,33,2,34,7,34,1,0,5,
  	0,72,8,0,10,0,12,0,75,9,0,1,1,1,1,3,1,79,8,1,1,2,1,2,1,3,1,3,1,3,1,3,
  	4,3,87,8,3,11,3,12,3,88,1,4,3,4,92,8,4,1,4,1,4,1,4,1,4,5,4,98,8,4,10,
  	4,12,4,101,9,4,1,4,1,4,1,5,1,5,3,5,107,8,5,1,6,1,6,1,6,3,6,112,8,6,1,
  	7,1,7,1,7,1,7,3,7,118,8,7,1,8,1,8,1,8,1,8,1,8,5,8,125,8,8,10,8,12,8,128,
  	9,8,3,8,130,8,8,1,8,3,8,133,8,8,1,9,1,9,1,9,1,9,1,9,1,9,5,9,141,8,9,10,
  	9,12,9,144,9,9,3,9,146,8,9,1,9,1,9,1,9,1,10,1,10,1,10,1,10,1,10,1,10,
  	1,10,1,10,5,10,159,8,10,10,10,12,10,162,9,10,3,10,164,8,10,1,11,1,11,
  	5,11,168,8,11,10,11,12,11,171,9,11,1,11,1,11,1,12,1,12,1,12,1,12,1,12,
  	1,12,1,12,1,12,1,12,1,12,1,12,3,12,186,8,12,1,13,1,13,1,13,1,13,1,13,
  	1,14,1,14,1,15,1,15,1,15,1,16,1,16,1,16,1,16,1,16,1,16,1,16,1,16,1,17,
  	1,17,1,17,1,17,1,17,1,17,1,18,1,18,1,18,1,18,1,18,1,18,1,19,1,19,1,19,
  	1,20,1,20,1,20,1,21,1,21,3,21,226,8,21,1,21,1,21,1,22,1,22,1,22,1,22,
  	1,22,5,22,235,8,22,10,22,12,22,238,9,22,1,23,1,23,1,23,1,23,1,23,1,23,
  	1,23,1,23,1,23,3,23,249,8,23,1,24,1,24,3,24,253,8,24,1,25,1,25,1,26,1,
  	26,1,26,1,26,1,26,4,26,262,8,26,11,26,12,26,263,1,27,1,27,1,27,1,27,1,
  	27,5,27,271,8,27,10,27,12,27,274,9,27,3,27,276,8,27,1,27,1,27,1,28,1,
  	28,1,29,1,29,1,29,5,29,285,8,29,10,29,12,29,288,9,29,1,30,1,30,1,30,5,
  	30,293,8,30,10,30,12,30,296,9,30,1,31,1,31,1,31,5,31,301,8,31,10,31,12,
  	31,304,9,31,1,32,1,32,1,32,5,32,309,8,32,10,32,12,32,312,9,32,1,33,1,
  	33,1,33,5,33,317,8,33,10,33,12,33,320,9,33,1,34,1,34,1,34,5,34,325,8,
  	34,10,34,12,34,328,9,34,1,34,0,0,35,0,2,4,6,8,10,12,14,16,18,20,22,24,
  	26,28,30,32,34,36,38,40,42,44,46,48,50,52,54,56,58,60,62,64,66,68,0,7,
  	3,0,5,5,7,7,9,9,2,0,12,13,23,23,1,0,35,36,1,0,17,18,1,0,19,22,1,0,12,
  	13,1,0,14,16,336,0,73,1,0,0,0,2,78,1,0,0,0,4,80,1,0,0,0,6,86,1,0,0,0,
  	8,91,1,0,0,0,10,106,1,0,0,0,12,108,1,0,0,0,14,113,1,0,0,0,16,132,1,0,
  	0,0,18,134,1,0,0,0,20,150,1,0,0,0,22,165,1,0,0,0,24,185,1,0,0,0,26,187,
  	1,0,0,0,28,192,1,0,0,0,30,194,1,0,0,0,32,197,1,0,0,0,34,205,1,0,0,0,36,
  	211,1,0,0,0,38,217,1,0,0,0,40,220,1,0,0,0,42,223,1,0,0,0,44,229,1,0,0,
  	0,46,248,1,0,0,0,48,252,1,0,0,0,50,254,1,0,0,0,52,256,1,0,0,0,54,265,
  	1,0,0,0,56,279,1,0,0,0,58,281,1,0,0,0,60,289,1,0,0,0,62,297,1,0,0,0,64,
  	305,1,0,0,0,66,313,1,0,0,0,68,321,1,0,0,0,70,72,3,2,1,0,71,70,1,0,0,0,
  	72,75,1,0,0,0,73,71,1,0,0,0,73,74,1,0,0,0,74,1,1,0,0,0,75,73,1,0,0,0,
  	76,79,3,8,4,0,77,79,3,18,9,0,78,76,1,0,0,0,78,77,1,0,0,0,79,3,1,0,0,0,
  	80,81,7,0,0,0,81,5,1,0,0,0,82,83,5,28,0,0,83,84,3,66,33,0,84,85,5,29,
  	0,0,85,87,1,0,0,0,86,82,1,0,0,0,87,88,1,0,0,0,88,86,1,0,0,0,88,89,1,0,
  	0,0,89,7,1,0,0,0,90,92,5,2,0,0,91,90,1,0,0,0,91,92,1,0,0,0,92,93,1,0,
  	0,0,93,94,3,4,2,0,94,99,3,10,5,0,95,96,5,32,0,0,96,98,3,10,5,0,97,95,
  	1,0,0,0,98,101,1,0,0,0,99,97,1,0,0,0,99,100,1,0,0,0,100,102,1,0,0,0,101,
  	99,1,0,0,0,102,103,5,33,0,0,103,9,1,0,0,0,104,107,3,12,6,0,105,107,3,
  	14,7,0,106,104,1,0,0,0,106,105,1,0,0,0,107,11,1,0,0,0,108,111,5,34,0,
  	0,109,110,5,11,0,0,110,112,3,66,33,0,111,109,1,0,0,0,111,112,1,0,0,0,
  	112,13,1,0,0,0,113,114,5,34,0,0,114,117,3,6,3,0,115,116,5,11,0,0,116,
  	118,3,16,8,0,117,115,1,0,0,0,117,118,1,0,0,0,118,15,1,0,0,0,119,133,3,
  	66,33,0,120,129,5,30,0,0,121,126,3,16,8,0,122,123,5,32,0,0,123,125,3,
  	16,8,0,124,122,1,0,0,0,125,128,1,0,0,0,126,124,1,0,0,0,126,127,1,0,0,
  	0,127,130,1,0,0,0,128,126,1,0,0,0,129,121,1,0,0,0,129,130,1,0,0,0,130,
  	131,1,0,0,0,131,133,5,31,0,0,132,119,1,0,0,0,132,120,1,0,0,0,133,17,1,
  	0,0,0,134,135,3,4,2,0,135,136,5,34,0,0,136,145,5,26,0,0,137,142,3,20,
  	10,0,138,139,5,32,0,0,139,141,3,20,10,0,140,138,1,0,0,0,141,144,1,0,0,
  	0,142,140,1,0,0,0,142,143,1,0,0,0,143,146,1,0,0,0,144,142,1,0,0,0,145,
  	137,1,0,0,0,145,146,1,0,0,0,146,147,1,0,0,0,147,148,5,27,0,0,148,149,
  	3,22,11,0,149,19,1,0,0,0,150,151,3,4,2,0,151,163,5,34,0,0,152,153,5,28,
  	0,0,153,160,5,29,0,0,154,155,5,28,0,0,155,156,3,66,33,0,156,157,5,29,
  	0,0,157,159,1,0,0,0,158,154,1,0,0,0,159,162,1,0,0,0,160,158,1,0,0,0,160,
  	161,1,0,0,0,161,164,1,0,0,0,162,160,1,0,0,0,163,152,1,0,0,0,163,164,1,
  	0,0,0,164,21,1,0,0,0,165,169,5,30,0,0,166,168,3,24,12,0,167,166,1,0,0,
  	0,168,171,1,0,0,0,169,167,1,0,0,0,169,170,1,0,0,0,170,172,1,0,0,0,171,
  	169,1,0,0,0,172,173,5,31,0,0,173,23,1,0,0,0,174,186,3,26,13,0,175,186,
  	3,8,4,0,176,186,3,30,15,0,177,186,3,34,17,0,178,186,3,32,16,0,179,186,
  	3,36,18,0,180,186,3,22,11,0,181,186,3,28,14,0,182,186,3,38,19,0,183,186,
  	3,40,20,0,184,186,3,42,21,0,185,174,1,0,0,0,185,175,1,0,0,0,185,176,1,
  	0,0,0,185,177,1,0,0,0,185,178,1,0,0,0,185,179,1,0,0,0,185,180,1,0,0,0,
  	185,181,1,0,0,0,185,182,1,0,0,0,185,183,1,0,0,0,185,184,1,0,0,0,186,25,
  	1,0,0,0,187,188,3,44,22,0,188,189,5,11,0,0,189,190,3,66,33,0,190,191,
  	5,33,0,0,191,27,1,0,0,0,192,193,5,33,0,0,193,29,1,0,0,0,194,195,3,66,
  	33,0,195,196,5,33,0,0,196,31,1,0,0,0,197,198,5,6,0,0,198,199,5,26,0,0,
  	199,200,3,58,29,0,200,201,5,27,0,0,201,202,3,24,12,0,202,203,5,4,0,0,
  	203,204,3,24,12,0,204,33,1,0,0,0,205,206,5,6,0,0,206,207,5,26,0,0,207,
  	208,3,58,29,0,208,209,5,27,0,0,209,210,3,24,12,0,210,35,1,0,0,0,211,212,
  	5,10,0,0,212,213,5,26,0,0,213,214,3,58,29,0,214,215,5,27,0,0,215,216,
  	3,24,12,0,216,37,1,0,0,0,217,218,5,1,0,0,218,219,5,33,0,0,219,39,1,0,
  	0,0,220,221,5,3,0,0,221,222,5,33,0,0,222,41,1,0,0,0,223,225,5,8,0,0,224,
  	226,3,66,33,0,225,224,1,0,0,0,225,226,1,0,0,0,226,227,1,0,0,0,227,228,
  	5,33,0,0,228,43,1,0,0,0,229,236,5,34,0,0,230,231,5,28,0,0,231,232,3,66,
  	33,0,232,233,5,29,0,0,233,235,1,0,0,0,234,230,1,0,0,0,235,238,1,0,0,0,
  	236,234,1,0,0,0,236,237,1,0,0,0,237,45,1,0,0,0,238,236,1,0,0,0,239,240,
  	7,1,0,0,240,249,3,46,23,0,241,242,5,26,0,0,242,243,3,66,33,0,243,244,
  	5,27,0,0,244,249,1,0,0,0,245,249,3,48,24,0,246,249,3,54,27,0,247,249,
  	3,56,28,0,248,239,1,0,0,0,248,241,1,0,0,0,248,245,1,0,0,0,248,246,1,0,
  	0,0,248,247,1,0,0,0,249,47,1,0,0,0,250,253,3,50,25,0,251,253,3,52,26,
  	0,252,250,1,0,0,0,252,251,1,0,0,0,253,49,1,0,0,0,254,255,5,34,0,0,255,
  	51,1,0,0,0,256,261,5,34,0,0,257,258,5,28,0,0,258,259,3,66,33,0,259,260,
  	5,29,0,0,260,262,1,0,0,0,261,257,1,0,0,0,262,263,1,0,0,0,263,261,1,0,
  	0,0,263,264,1,0,0,0,264,53,1,0,0,0,265,266,5,34,0,0,266,275,5,26,0,0,
  	267,272,3,66,33,0,268,269,5,32,0,0,269,271,3,66,33,0,270,268,1,0,0,0,
  	271,274,1,0,0,0,272,270,1,0,0,0,272,273,1,0,0,0,273,276,1,0,0,0,274,272,
  	1,0,0,0,275,267,1,0,0,0,275,276,1,0,0,0,276,277,1,0,0,0,277,278,5,27,
  	0,0,278,55,1,0,0,0,279,280,7,2,0,0,280,57,1,0,0,0,281,286,3,60,30,0,282,
  	283,5,25,0,0,283,285,3,60,30,0,284,282,1,0,0,0,285,288,1,0,0,0,286,284,
  	1,0,0,0,286,287,1,0,0,0,287,59,1,0,0,0,288,286,1,0,0,0,289,294,3,62,31,
  	0,290,291,5,24,0,0,291,293,3,62,31,0,292,290,1,0,0,0,293,296,1,0,0,0,
  	294,292,1,0,0,0,294,295,1,0,0,0,295,61,1,0,0,0,296,294,1,0,0,0,297,302,
  	3,64,32,0,298,299,7,3,0,0,299,301,3,64,32,0,300,298,1,0,0,0,301,304,1,
  	0,0,0,302,300,1,0,0,0,302,303,1,0,0,0,303,63,1,0,0,0,304,302,1,0,0,0,
  	305,310,3,66,33,0,306,307,7,4,0,0,307,309,3,66,33,0,308,306,1,0,0,0,309,
  	312,1,0,0,0,310,308,1,0,0,0,310,311,1,0,0,0,311,65,1,0,0,0,312,310,1,
  	0,0,0,313,318,3,68,34,0,314,315,7,5,0,0,315,317,3,68,34,0,316,314,1,0,
  	0,0,317,320,1,0,0,0,318,316,1,0,0,0,318,319,1,0,0,0,319,67,1,0,0,0,320,
  	318,1,0,0,0,321,326,3,46,23,0,322,323,7,6,0,0,323,325,3,46,23,0,324,322,
  	1,0,0,0,325,328,1,0,0,0,326,324,1,0,0,0,326,327,1,0,0,0,327,69,1,0,0,
  	0,328,326,1,0,0,0,30,73,78,88,91,99,106,111,117,126,129,132,142,145,160,
  	163,169,185,225,236,248,252,263,272,275,286,294,302,310,318,326
  };
  staticData->serializedATN = antlr4::atn::SerializedATNView(serializedATNSegment, sizeof(serializedATNSegment) / sizeof(serializedATNSegment[0]));

  antlr4::atn::ATNDeserializer deserializer;
  staticData->atn = deserializer.deserialize(staticData->serializedATN);

  const size_t count = staticData->atn->getNumberOfDecisions();
  staticData->decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    staticData->decisionToDFA.emplace_back(staticData->atn->getDecisionState(i), i);
  }
  sysyParserStaticData = staticData.release();
}

}

SysYParser::SysYParser(TokenStream *input) : SysYParser(input, antlr4::atn::ParserATNSimulatorOptions()) {}

SysYParser::SysYParser(TokenStream *input, const antlr4::atn::ParserATNSimulatorOptions &options) : Parser(input) {
  SysYParser::initialize();
  _interpreter = new atn::ParserATNSimulator(this, *sysyParserStaticData->atn, sysyParserStaticData->decisionToDFA, sysyParserStaticData->sharedContextCache, options);
}

SysYParser::~SysYParser() {
  delete _interpreter;
}

const atn::ATN& SysYParser::getATN() const {
  return *sysyParserStaticData->atn;
}

std::string SysYParser::getGrammarFileName() const {
  return "SysY.g4";
}

const std::vector<std::string>& SysYParser::getRuleNames() const {
  return sysyParserStaticData->ruleNames;
}

const dfa::Vocabulary& SysYParser::getVocabulary() const {
  return sysyParserStaticData->vocabulary;
}

antlr4::atn::SerializedATNView SysYParser::getSerializedATN() const {
  return sysyParserStaticData->serializedATN;
}


//----------------- RootContext ------------------------------------------------------------------

SysYParser::RootContext::RootContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<SysYParser::CompUnitContext *> SysYParser::RootContext::compUnit() {
  return getRuleContexts<SysYParser::CompUnitContext>();
}

SysYParser::CompUnitContext* SysYParser::RootContext::compUnit(size_t i) {
  return getRuleContext<SysYParser::CompUnitContext>(i);
}


size_t SysYParser::RootContext::getRuleIndex() const {
  return SysYParser::RuleRoot;
}


std::any SysYParser::RootContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitRoot(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::RootContext* SysYParser::root() {
  RootContext *_localctx = _tracker.createInstance<RootContext>(_ctx, getState());
  enterRule(_localctx, 0, SysYParser::RuleRoot);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(73);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 676) != 0)) {
      setState(70);
      compUnit();
      setState(75);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- CompUnitContext ------------------------------------------------------------------

SysYParser::CompUnitContext::CompUnitContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SysYParser::VarDeclContext* SysYParser::CompUnitContext::varDecl() {
  return getRuleContext<SysYParser::VarDeclContext>(0);
}

SysYParser::FuncDefContext* SysYParser::CompUnitContext::funcDef() {
  return getRuleContext<SysYParser::FuncDefContext>(0);
}


size_t SysYParser::CompUnitContext::getRuleIndex() const {
  return SysYParser::RuleCompUnit;
}


std::any SysYParser::CompUnitContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitCompUnit(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::CompUnitContext* SysYParser::compUnit() {
  CompUnitContext *_localctx = _tracker.createInstance<CompUnitContext>(_ctx, getState());
  enterRule(_localctx, 2, SysYParser::RuleCompUnit);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(78);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 1, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(76);
      varDecl();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(77);
      funcDef();
      break;
    }

    default:
      break;
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- TypeContext ------------------------------------------------------------------

SysYParser::TypeContext::TypeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SysYParser::TypeContext::INT() {
  return getToken(SysYParser::INT, 0);
}

tree::TerminalNode* SysYParser::TypeContext::FLOAT() {
  return getToken(SysYParser::FLOAT, 0);
}

tree::TerminalNode* SysYParser::TypeContext::VOID() {
  return getToken(SysYParser::VOID, 0);
}


size_t SysYParser::TypeContext::getRuleIndex() const {
  return SysYParser::RuleType;
}


std::any SysYParser::TypeContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitType(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::TypeContext* SysYParser::type() {
  TypeContext *_localctx = _tracker.createInstance<TypeContext>(_ctx, getState());
  enterRule(_localctx, 4, SysYParser::RuleType);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(80);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 672) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- DimensionsContext ------------------------------------------------------------------

SysYParser::DimensionsContext::DimensionsContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<tree::TerminalNode *> SysYParser::DimensionsContext::LB() {
  return getTokens(SysYParser::LB);
}

tree::TerminalNode* SysYParser::DimensionsContext::LB(size_t i) {
  return getToken(SysYParser::LB, i);
}

std::vector<SysYParser::AdditiveExpContext *> SysYParser::DimensionsContext::additiveExp() {
  return getRuleContexts<SysYParser::AdditiveExpContext>();
}

SysYParser::AdditiveExpContext* SysYParser::DimensionsContext::additiveExp(size_t i) {
  return getRuleContext<SysYParser::AdditiveExpContext>(i);
}

std::vector<tree::TerminalNode *> SysYParser::DimensionsContext::RB() {
  return getTokens(SysYParser::RB);
}

tree::TerminalNode* SysYParser::DimensionsContext::RB(size_t i) {
  return getToken(SysYParser::RB, i);
}


size_t SysYParser::DimensionsContext::getRuleIndex() const {
  return SysYParser::RuleDimensions;
}


std::any SysYParser::DimensionsContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitDimensions(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::DimensionsContext* SysYParser::dimensions() {
  DimensionsContext *_localctx = _tracker.createInstance<DimensionsContext>(_ctx, getState());
  enterRule(_localctx, 6, SysYParser::RuleDimensions);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(86); 
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(82);
      match(SysYParser::LB);
      setState(83);
      additiveExp();
      setState(84);
      match(SysYParser::RB);
      setState(88); 
      _errHandler->sync(this);
      _la = _input->LA(1);
    } while (_la == SysYParser::LB);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- VarDeclContext ------------------------------------------------------------------

SysYParser::VarDeclContext::VarDeclContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SysYParser::TypeContext* SysYParser::VarDeclContext::type() {
  return getRuleContext<SysYParser::TypeContext>(0);
}

std::vector<SysYParser::VarDefContext *> SysYParser::VarDeclContext::varDef() {
  return getRuleContexts<SysYParser::VarDefContext>();
}

SysYParser::VarDefContext* SysYParser::VarDeclContext::varDef(size_t i) {
  return getRuleContext<SysYParser::VarDefContext>(i);
}

tree::TerminalNode* SysYParser::VarDeclContext::SEMI() {
  return getToken(SysYParser::SEMI, 0);
}

tree::TerminalNode* SysYParser::VarDeclContext::CONST() {
  return getToken(SysYParser::CONST, 0);
}

std::vector<tree::TerminalNode *> SysYParser::VarDeclContext::COMMA() {
  return getTokens(SysYParser::COMMA);
}

tree::TerminalNode* SysYParser::VarDeclContext::COMMA(size_t i) {
  return getToken(SysYParser::COMMA, i);
}


size_t SysYParser::VarDeclContext::getRuleIndex() const {
  return SysYParser::RuleVarDecl;
}


std::any SysYParser::VarDeclContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitVarDecl(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::VarDeclContext* SysYParser::varDecl() {
  VarDeclContext *_localctx = _tracker.createInstance<VarDeclContext>(_ctx, getState());
  enterRule(_localctx, 8, SysYParser::RuleVarDecl);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(91);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == SysYParser::CONST) {
      setState(90);
      match(SysYParser::CONST);
    }
    setState(93);
    type();
    setState(94);
    varDef();
    setState(99);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == SysYParser::COMMA) {
      setState(95);
      match(SysYParser::COMMA);
      setState(96);
      varDef();
      setState(101);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(102);
    match(SysYParser::SEMI);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- VarDefContext ------------------------------------------------------------------

SysYParser::VarDefContext::VarDefContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SysYParser::ScalarVarDefContext* SysYParser::VarDefContext::scalarVarDef() {
  return getRuleContext<SysYParser::ScalarVarDefContext>(0);
}

SysYParser::ArrayVarDefContext* SysYParser::VarDefContext::arrayVarDef() {
  return getRuleContext<SysYParser::ArrayVarDefContext>(0);
}


size_t SysYParser::VarDefContext::getRuleIndex() const {
  return SysYParser::RuleVarDef;
}


std::any SysYParser::VarDefContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitVarDef(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::VarDefContext* SysYParser::varDef() {
  VarDefContext *_localctx = _tracker.createInstance<VarDefContext>(_ctx, getState());
  enterRule(_localctx, 10, SysYParser::RuleVarDef);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(106);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 5, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(104);
      scalarVarDef();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(105);
      arrayVarDef();
      break;
    }

    default:
      break;
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ScalarVarDefContext ------------------------------------------------------------------

SysYParser::ScalarVarDefContext::ScalarVarDefContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SysYParser::ScalarVarDefContext::Ident() {
  return getToken(SysYParser::Ident, 0);
}

tree::TerminalNode* SysYParser::ScalarVarDefContext::ASSIGN() {
  return getToken(SysYParser::ASSIGN, 0);
}

SysYParser::AdditiveExpContext* SysYParser::ScalarVarDefContext::additiveExp() {
  return getRuleContext<SysYParser::AdditiveExpContext>(0);
}


size_t SysYParser::ScalarVarDefContext::getRuleIndex() const {
  return SysYParser::RuleScalarVarDef;
}


std::any SysYParser::ScalarVarDefContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitScalarVarDef(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::ScalarVarDefContext* SysYParser::scalarVarDef() {
  ScalarVarDefContext *_localctx = _tracker.createInstance<ScalarVarDefContext>(_ctx, getState());
  enterRule(_localctx, 12, SysYParser::RuleScalarVarDef);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(108);
    match(SysYParser::Ident);
    setState(111);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == SysYParser::ASSIGN) {
      setState(109);
      match(SysYParser::ASSIGN);
      setState(110);
      additiveExp();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ArrayVarDefContext ------------------------------------------------------------------

SysYParser::ArrayVarDefContext::ArrayVarDefContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SysYParser::ArrayVarDefContext::Ident() {
  return getToken(SysYParser::Ident, 0);
}

SysYParser::DimensionsContext* SysYParser::ArrayVarDefContext::dimensions() {
  return getRuleContext<SysYParser::DimensionsContext>(0);
}

tree::TerminalNode* SysYParser::ArrayVarDefContext::ASSIGN() {
  return getToken(SysYParser::ASSIGN, 0);
}

SysYParser::InitValContext* SysYParser::ArrayVarDefContext::initVal() {
  return getRuleContext<SysYParser::InitValContext>(0);
}


size_t SysYParser::ArrayVarDefContext::getRuleIndex() const {
  return SysYParser::RuleArrayVarDef;
}


std::any SysYParser::ArrayVarDefContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitArrayVarDef(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::ArrayVarDefContext* SysYParser::arrayVarDef() {
  ArrayVarDefContext *_localctx = _tracker.createInstance<ArrayVarDefContext>(_ctx, getState());
  enterRule(_localctx, 14, SysYParser::RuleArrayVarDef);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(113);
    match(SysYParser::Ident);
    setState(114);
    dimensions();
    setState(117);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == SysYParser::ASSIGN) {
      setState(115);
      match(SysYParser::ASSIGN);
      setState(116);
      initVal();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- InitValContext ------------------------------------------------------------------

SysYParser::InitValContext::InitValContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SysYParser::AdditiveExpContext* SysYParser::InitValContext::additiveExp() {
  return getRuleContext<SysYParser::AdditiveExpContext>(0);
}

tree::TerminalNode* SysYParser::InitValContext::LC() {
  return getToken(SysYParser::LC, 0);
}

tree::TerminalNode* SysYParser::InitValContext::RC() {
  return getToken(SysYParser::RC, 0);
}

std::vector<SysYParser::InitValContext *> SysYParser::InitValContext::initVal() {
  return getRuleContexts<SysYParser::InitValContext>();
}

SysYParser::InitValContext* SysYParser::InitValContext::initVal(size_t i) {
  return getRuleContext<SysYParser::InitValContext>(i);
}

std::vector<tree::TerminalNode *> SysYParser::InitValContext::COMMA() {
  return getTokens(SysYParser::COMMA);
}

tree::TerminalNode* SysYParser::InitValContext::COMMA(size_t i) {
  return getToken(SysYParser::COMMA, i);
}


size_t SysYParser::InitValContext::getRuleIndex() const {
  return SysYParser::RuleInitVal;
}


std::any SysYParser::InitValContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitInitVal(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::InitValContext* SysYParser::initVal() {
  InitValContext *_localctx = _tracker.createInstance<InitValContext>(_ctx, getState());
  enterRule(_localctx, 16, SysYParser::RuleInitVal);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(132);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case SysYParser::ADD:
      case SysYParser::SUB:
      case SysYParser::LNOT:
      case SysYParser::LP:
      case SysYParser::Ident:
      case SysYParser::IntConst:
      case SysYParser::FloatConst: {
        enterOuterAlt(_localctx, 1);
        setState(119);
        additiveExp();
        break;
      }

      case SysYParser::LC: {
        enterOuterAlt(_localctx, 2);
        setState(120);
        match(SysYParser::LC);
        setState(129);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if ((((_la & ~ 0x3fULL) == 0) &&
          ((1ULL << _la) & 121408335872) != 0)) {
          setState(121);
          initVal();
          setState(126);
          _errHandler->sync(this);
          _la = _input->LA(1);
          while (_la == SysYParser::COMMA) {
            setState(122);
            match(SysYParser::COMMA);
            setState(123);
            initVal();
            setState(128);
            _errHandler->sync(this);
            _la = _input->LA(1);
          }
        }
        setState(131);
        match(SysYParser::RC);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- FuncDefContext ------------------------------------------------------------------

SysYParser::FuncDefContext::FuncDefContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SysYParser::TypeContext* SysYParser::FuncDefContext::type() {
  return getRuleContext<SysYParser::TypeContext>(0);
}

tree::TerminalNode* SysYParser::FuncDefContext::Ident() {
  return getToken(SysYParser::Ident, 0);
}

tree::TerminalNode* SysYParser::FuncDefContext::LP() {
  return getToken(SysYParser::LP, 0);
}

tree::TerminalNode* SysYParser::FuncDefContext::RP() {
  return getToken(SysYParser::RP, 0);
}

SysYParser::BlockStmtContext* SysYParser::FuncDefContext::blockStmt() {
  return getRuleContext<SysYParser::BlockStmtContext>(0);
}

std::vector<SysYParser::FuncArgContext *> SysYParser::FuncDefContext::funcArg() {
  return getRuleContexts<SysYParser::FuncArgContext>();
}

SysYParser::FuncArgContext* SysYParser::FuncDefContext::funcArg(size_t i) {
  return getRuleContext<SysYParser::FuncArgContext>(i);
}

std::vector<tree::TerminalNode *> SysYParser::FuncDefContext::COMMA() {
  return getTokens(SysYParser::COMMA);
}

tree::TerminalNode* SysYParser::FuncDefContext::COMMA(size_t i) {
  return getToken(SysYParser::COMMA, i);
}


size_t SysYParser::FuncDefContext::getRuleIndex() const {
  return SysYParser::RuleFuncDef;
}


std::any SysYParser::FuncDefContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitFuncDef(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::FuncDefContext* SysYParser::funcDef() {
  FuncDefContext *_localctx = _tracker.createInstance<FuncDefContext>(_ctx, getState());
  enterRule(_localctx, 18, SysYParser::RuleFuncDef);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(134);
    type();
    setState(135);
    match(SysYParser::Ident);
    setState(136);
    match(SysYParser::LP);
    setState(145);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 672) != 0)) {
      setState(137);
      funcArg();
      setState(142);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == SysYParser::COMMA) {
        setState(138);
        match(SysYParser::COMMA);
        setState(139);
        funcArg();
        setState(144);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
    }
    setState(147);
    match(SysYParser::RP);
    setState(148);
    blockStmt();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- FuncArgContext ------------------------------------------------------------------

SysYParser::FuncArgContext::FuncArgContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SysYParser::TypeContext* SysYParser::FuncArgContext::type() {
  return getRuleContext<SysYParser::TypeContext>(0);
}

tree::TerminalNode* SysYParser::FuncArgContext::Ident() {
  return getToken(SysYParser::Ident, 0);
}

std::vector<tree::TerminalNode *> SysYParser::FuncArgContext::LB() {
  return getTokens(SysYParser::LB);
}

tree::TerminalNode* SysYParser::FuncArgContext::LB(size_t i) {
  return getToken(SysYParser::LB, i);
}

std::vector<tree::TerminalNode *> SysYParser::FuncArgContext::RB() {
  return getTokens(SysYParser::RB);
}

tree::TerminalNode* SysYParser::FuncArgContext::RB(size_t i) {
  return getToken(SysYParser::RB, i);
}

std::vector<SysYParser::AdditiveExpContext *> SysYParser::FuncArgContext::additiveExp() {
  return getRuleContexts<SysYParser::AdditiveExpContext>();
}

SysYParser::AdditiveExpContext* SysYParser::FuncArgContext::additiveExp(size_t i) {
  return getRuleContext<SysYParser::AdditiveExpContext>(i);
}


size_t SysYParser::FuncArgContext::getRuleIndex() const {
  return SysYParser::RuleFuncArg;
}


std::any SysYParser::FuncArgContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitFuncArg(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::FuncArgContext* SysYParser::funcArg() {
  FuncArgContext *_localctx = _tracker.createInstance<FuncArgContext>(_ctx, getState());
  enterRule(_localctx, 20, SysYParser::RuleFuncArg);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(150);
    type();
    setState(151);
    match(SysYParser::Ident);
    setState(163);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == SysYParser::LB) {
      setState(152);
      match(SysYParser::LB);
      setState(153);
      match(SysYParser::RB);
      setState(160);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == SysYParser::LB) {
        setState(154);
        match(SysYParser::LB);
        setState(155);
        additiveExp();
        setState(156);
        match(SysYParser::RB);
        setState(162);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- BlockStmtContext ------------------------------------------------------------------

SysYParser::BlockStmtContext::BlockStmtContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SysYParser::BlockStmtContext::LC() {
  return getToken(SysYParser::LC, 0);
}

tree::TerminalNode* SysYParser::BlockStmtContext::RC() {
  return getToken(SysYParser::RC, 0);
}

std::vector<SysYParser::StmtContext *> SysYParser::BlockStmtContext::stmt() {
  return getRuleContexts<SysYParser::StmtContext>();
}

SysYParser::StmtContext* SysYParser::BlockStmtContext::stmt(size_t i) {
  return getRuleContext<SysYParser::StmtContext>(i);
}


size_t SysYParser::BlockStmtContext::getRuleIndex() const {
  return SysYParser::RuleBlockStmt;
}


std::any SysYParser::BlockStmtContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitBlockStmt(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::BlockStmtContext* SysYParser::blockStmt() {
  BlockStmtContext *_localctx = _tracker.createInstance<BlockStmtContext>(_ctx, getState());
  enterRule(_localctx, 22, SysYParser::RuleBlockStmt);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(165);
    match(SysYParser::LC);
    setState(169);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 129998272494) != 0)) {
      setState(166);
      stmt();
      setState(171);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(172);
    match(SysYParser::RC);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- StmtContext ------------------------------------------------------------------

SysYParser::StmtContext::StmtContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SysYParser::AssignStmtContext* SysYParser::StmtContext::assignStmt() {
  return getRuleContext<SysYParser::AssignStmtContext>(0);
}

SysYParser::VarDeclContext* SysYParser::StmtContext::varDecl() {
  return getRuleContext<SysYParser::VarDeclContext>(0);
}

SysYParser::ExpStmtContext* SysYParser::StmtContext::expStmt() {
  return getRuleContext<SysYParser::ExpStmtContext>(0);
}

SysYParser::IfStmtContext* SysYParser::StmtContext::ifStmt() {
  return getRuleContext<SysYParser::IfStmtContext>(0);
}

SysYParser::IfElseStmtContext* SysYParser::StmtContext::ifElseStmt() {
  return getRuleContext<SysYParser::IfElseStmtContext>(0);
}

SysYParser::WhileStmtContext* SysYParser::StmtContext::whileStmt() {
  return getRuleContext<SysYParser::WhileStmtContext>(0);
}

SysYParser::BlockStmtContext* SysYParser::StmtContext::blockStmt() {
  return getRuleContext<SysYParser::BlockStmtContext>(0);
}

SysYParser::BlankStmtContext* SysYParser::StmtContext::blankStmt() {
  return getRuleContext<SysYParser::BlankStmtContext>(0);
}

SysYParser::BreakStmtContext* SysYParser::StmtContext::breakStmt() {
  return getRuleContext<SysYParser::BreakStmtContext>(0);
}

SysYParser::ContinueStmtContext* SysYParser::StmtContext::continueStmt() {
  return getRuleContext<SysYParser::ContinueStmtContext>(0);
}

SysYParser::RetStmtContext* SysYParser::StmtContext::retStmt() {
  return getRuleContext<SysYParser::RetStmtContext>(0);
}


size_t SysYParser::StmtContext::getRuleIndex() const {
  return SysYParser::RuleStmt;
}


std::any SysYParser::StmtContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitStmt(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::StmtContext* SysYParser::stmt() {
  StmtContext *_localctx = _tracker.createInstance<StmtContext>(_ctx, getState());
  enterRule(_localctx, 24, SysYParser::RuleStmt);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(185);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 16, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(174);
      assignStmt();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(175);
      varDecl();
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(176);
      expStmt();
      break;
    }

    case 4: {
      enterOuterAlt(_localctx, 4);
      setState(177);
      ifStmt();
      break;
    }

    case 5: {
      enterOuterAlt(_localctx, 5);
      setState(178);
      ifElseStmt();
      break;
    }

    case 6: {
      enterOuterAlt(_localctx, 6);
      setState(179);
      whileStmt();
      break;
    }

    case 7: {
      enterOuterAlt(_localctx, 7);
      setState(180);
      blockStmt();
      break;
    }

    case 8: {
      enterOuterAlt(_localctx, 8);
      setState(181);
      blankStmt();
      break;
    }

    case 9: {
      enterOuterAlt(_localctx, 9);
      setState(182);
      breakStmt();
      break;
    }

    case 10: {
      enterOuterAlt(_localctx, 10);
      setState(183);
      continueStmt();
      break;
    }

    case 11: {
      enterOuterAlt(_localctx, 11);
      setState(184);
      retStmt();
      break;
    }

    default:
      break;
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- AssignStmtContext ------------------------------------------------------------------

SysYParser::AssignStmtContext::AssignStmtContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SysYParser::LValContext* SysYParser::AssignStmtContext::lVal() {
  return getRuleContext<SysYParser::LValContext>(0);
}

tree::TerminalNode* SysYParser::AssignStmtContext::ASSIGN() {
  return getToken(SysYParser::ASSIGN, 0);
}

SysYParser::AdditiveExpContext* SysYParser::AssignStmtContext::additiveExp() {
  return getRuleContext<SysYParser::AdditiveExpContext>(0);
}

tree::TerminalNode* SysYParser::AssignStmtContext::SEMI() {
  return getToken(SysYParser::SEMI, 0);
}


size_t SysYParser::AssignStmtContext::getRuleIndex() const {
  return SysYParser::RuleAssignStmt;
}


std::any SysYParser::AssignStmtContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitAssignStmt(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::AssignStmtContext* SysYParser::assignStmt() {
  AssignStmtContext *_localctx = _tracker.createInstance<AssignStmtContext>(_ctx, getState());
  enterRule(_localctx, 26, SysYParser::RuleAssignStmt);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(187);
    lVal();
    setState(188);
    match(SysYParser::ASSIGN);
    setState(189);
    additiveExp();
    setState(190);
    match(SysYParser::SEMI);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- BlankStmtContext ------------------------------------------------------------------

SysYParser::BlankStmtContext::BlankStmtContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SysYParser::BlankStmtContext::SEMI() {
  return getToken(SysYParser::SEMI, 0);
}


size_t SysYParser::BlankStmtContext::getRuleIndex() const {
  return SysYParser::RuleBlankStmt;
}


std::any SysYParser::BlankStmtContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitBlankStmt(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::BlankStmtContext* SysYParser::blankStmt() {
  BlankStmtContext *_localctx = _tracker.createInstance<BlankStmtContext>(_ctx, getState());
  enterRule(_localctx, 28, SysYParser::RuleBlankStmt);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(192);
    match(SysYParser::SEMI);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ExpStmtContext ------------------------------------------------------------------

SysYParser::ExpStmtContext::ExpStmtContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SysYParser::AdditiveExpContext* SysYParser::ExpStmtContext::additiveExp() {
  return getRuleContext<SysYParser::AdditiveExpContext>(0);
}

tree::TerminalNode* SysYParser::ExpStmtContext::SEMI() {
  return getToken(SysYParser::SEMI, 0);
}


size_t SysYParser::ExpStmtContext::getRuleIndex() const {
  return SysYParser::RuleExpStmt;
}


std::any SysYParser::ExpStmtContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitExpStmt(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::ExpStmtContext* SysYParser::expStmt() {
  ExpStmtContext *_localctx = _tracker.createInstance<ExpStmtContext>(_ctx, getState());
  enterRule(_localctx, 30, SysYParser::RuleExpStmt);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(194);
    additiveExp();
    setState(195);
    match(SysYParser::SEMI);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- IfElseStmtContext ------------------------------------------------------------------

SysYParser::IfElseStmtContext::IfElseStmtContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SysYParser::IfElseStmtContext::IF() {
  return getToken(SysYParser::IF, 0);
}

tree::TerminalNode* SysYParser::IfElseStmtContext::LP() {
  return getToken(SysYParser::LP, 0);
}

SysYParser::LorExpContext* SysYParser::IfElseStmtContext::lorExp() {
  return getRuleContext<SysYParser::LorExpContext>(0);
}

tree::TerminalNode* SysYParser::IfElseStmtContext::RP() {
  return getToken(SysYParser::RP, 0);
}

std::vector<SysYParser::StmtContext *> SysYParser::IfElseStmtContext::stmt() {
  return getRuleContexts<SysYParser::StmtContext>();
}

SysYParser::StmtContext* SysYParser::IfElseStmtContext::stmt(size_t i) {
  return getRuleContext<SysYParser::StmtContext>(i);
}

tree::TerminalNode* SysYParser::IfElseStmtContext::ELSE() {
  return getToken(SysYParser::ELSE, 0);
}


size_t SysYParser::IfElseStmtContext::getRuleIndex() const {
  return SysYParser::RuleIfElseStmt;
}


std::any SysYParser::IfElseStmtContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitIfElseStmt(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::IfElseStmtContext* SysYParser::ifElseStmt() {
  IfElseStmtContext *_localctx = _tracker.createInstance<IfElseStmtContext>(_ctx, getState());
  enterRule(_localctx, 32, SysYParser::RuleIfElseStmt);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(197);
    match(SysYParser::IF);
    setState(198);
    match(SysYParser::LP);
    setState(199);
    lorExp();
    setState(200);
    match(SysYParser::RP);
    setState(201);
    stmt();
    setState(202);
    match(SysYParser::ELSE);
    setState(203);
    stmt();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- IfStmtContext ------------------------------------------------------------------

SysYParser::IfStmtContext::IfStmtContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SysYParser::IfStmtContext::IF() {
  return getToken(SysYParser::IF, 0);
}

tree::TerminalNode* SysYParser::IfStmtContext::LP() {
  return getToken(SysYParser::LP, 0);
}

SysYParser::LorExpContext* SysYParser::IfStmtContext::lorExp() {
  return getRuleContext<SysYParser::LorExpContext>(0);
}

tree::TerminalNode* SysYParser::IfStmtContext::RP() {
  return getToken(SysYParser::RP, 0);
}

SysYParser::StmtContext* SysYParser::IfStmtContext::stmt() {
  return getRuleContext<SysYParser::StmtContext>(0);
}


size_t SysYParser::IfStmtContext::getRuleIndex() const {
  return SysYParser::RuleIfStmt;
}


std::any SysYParser::IfStmtContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitIfStmt(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::IfStmtContext* SysYParser::ifStmt() {
  IfStmtContext *_localctx = _tracker.createInstance<IfStmtContext>(_ctx, getState());
  enterRule(_localctx, 34, SysYParser::RuleIfStmt);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(205);
    match(SysYParser::IF);
    setState(206);
    match(SysYParser::LP);
    setState(207);
    lorExp();
    setState(208);
    match(SysYParser::RP);
    setState(209);
    stmt();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- WhileStmtContext ------------------------------------------------------------------

SysYParser::WhileStmtContext::WhileStmtContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SysYParser::WhileStmtContext::WHILE() {
  return getToken(SysYParser::WHILE, 0);
}

tree::TerminalNode* SysYParser::WhileStmtContext::LP() {
  return getToken(SysYParser::LP, 0);
}

SysYParser::LorExpContext* SysYParser::WhileStmtContext::lorExp() {
  return getRuleContext<SysYParser::LorExpContext>(0);
}

tree::TerminalNode* SysYParser::WhileStmtContext::RP() {
  return getToken(SysYParser::RP, 0);
}

SysYParser::StmtContext* SysYParser::WhileStmtContext::stmt() {
  return getRuleContext<SysYParser::StmtContext>(0);
}


size_t SysYParser::WhileStmtContext::getRuleIndex() const {
  return SysYParser::RuleWhileStmt;
}


std::any SysYParser::WhileStmtContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitWhileStmt(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::WhileStmtContext* SysYParser::whileStmt() {
  WhileStmtContext *_localctx = _tracker.createInstance<WhileStmtContext>(_ctx, getState());
  enterRule(_localctx, 36, SysYParser::RuleWhileStmt);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(211);
    match(SysYParser::WHILE);
    setState(212);
    match(SysYParser::LP);
    setState(213);
    lorExp();
    setState(214);
    match(SysYParser::RP);
    setState(215);
    stmt();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- BreakStmtContext ------------------------------------------------------------------

SysYParser::BreakStmtContext::BreakStmtContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SysYParser::BreakStmtContext::BREAK() {
  return getToken(SysYParser::BREAK, 0);
}

tree::TerminalNode* SysYParser::BreakStmtContext::SEMI() {
  return getToken(SysYParser::SEMI, 0);
}


size_t SysYParser::BreakStmtContext::getRuleIndex() const {
  return SysYParser::RuleBreakStmt;
}


std::any SysYParser::BreakStmtContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitBreakStmt(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::BreakStmtContext* SysYParser::breakStmt() {
  BreakStmtContext *_localctx = _tracker.createInstance<BreakStmtContext>(_ctx, getState());
  enterRule(_localctx, 38, SysYParser::RuleBreakStmt);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(217);
    match(SysYParser::BREAK);
    setState(218);
    match(SysYParser::SEMI);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ContinueStmtContext ------------------------------------------------------------------

SysYParser::ContinueStmtContext::ContinueStmtContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SysYParser::ContinueStmtContext::CONTINUE() {
  return getToken(SysYParser::CONTINUE, 0);
}

tree::TerminalNode* SysYParser::ContinueStmtContext::SEMI() {
  return getToken(SysYParser::SEMI, 0);
}


size_t SysYParser::ContinueStmtContext::getRuleIndex() const {
  return SysYParser::RuleContinueStmt;
}


std::any SysYParser::ContinueStmtContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitContinueStmt(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::ContinueStmtContext* SysYParser::continueStmt() {
  ContinueStmtContext *_localctx = _tracker.createInstance<ContinueStmtContext>(_ctx, getState());
  enterRule(_localctx, 40, SysYParser::RuleContinueStmt);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(220);
    match(SysYParser::CONTINUE);
    setState(221);
    match(SysYParser::SEMI);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- RetStmtContext ------------------------------------------------------------------

SysYParser::RetStmtContext::RetStmtContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SysYParser::RetStmtContext::RETURN() {
  return getToken(SysYParser::RETURN, 0);
}

tree::TerminalNode* SysYParser::RetStmtContext::SEMI() {
  return getToken(SysYParser::SEMI, 0);
}

SysYParser::AdditiveExpContext* SysYParser::RetStmtContext::additiveExp() {
  return getRuleContext<SysYParser::AdditiveExpContext>(0);
}


size_t SysYParser::RetStmtContext::getRuleIndex() const {
  return SysYParser::RuleRetStmt;
}


std::any SysYParser::RetStmtContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitRetStmt(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::RetStmtContext* SysYParser::retStmt() {
  RetStmtContext *_localctx = _tracker.createInstance<RetStmtContext>(_ctx, getState());
  enterRule(_localctx, 42, SysYParser::RuleRetStmt);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(223);
    match(SysYParser::RETURN);
    setState(225);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 120334594048) != 0)) {
      setState(224);
      additiveExp();
    }
    setState(227);
    match(SysYParser::SEMI);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- LValContext ------------------------------------------------------------------

SysYParser::LValContext::LValContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SysYParser::LValContext::Ident() {
  return getToken(SysYParser::Ident, 0);
}

std::vector<tree::TerminalNode *> SysYParser::LValContext::LB() {
  return getTokens(SysYParser::LB);
}

tree::TerminalNode* SysYParser::LValContext::LB(size_t i) {
  return getToken(SysYParser::LB, i);
}

std::vector<SysYParser::AdditiveExpContext *> SysYParser::LValContext::additiveExp() {
  return getRuleContexts<SysYParser::AdditiveExpContext>();
}

SysYParser::AdditiveExpContext* SysYParser::LValContext::additiveExp(size_t i) {
  return getRuleContext<SysYParser::AdditiveExpContext>(i);
}

std::vector<tree::TerminalNode *> SysYParser::LValContext::RB() {
  return getTokens(SysYParser::RB);
}

tree::TerminalNode* SysYParser::LValContext::RB(size_t i) {
  return getToken(SysYParser::RB, i);
}


size_t SysYParser::LValContext::getRuleIndex() const {
  return SysYParser::RuleLVal;
}


std::any SysYParser::LValContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitLVal(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::LValContext* SysYParser::lVal() {
  LValContext *_localctx = _tracker.createInstance<LValContext>(_ctx, getState());
  enterRule(_localctx, 44, SysYParser::RuleLVal);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(229);
    match(SysYParser::Ident);
    setState(236);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == SysYParser::LB) {
      setState(230);
      match(SysYParser::LB);
      setState(231);
      additiveExp();
      setState(232);
      match(SysYParser::RB);
      setState(238);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- UnaryExpContext ------------------------------------------------------------------

SysYParser::UnaryExpContext::UnaryExpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SysYParser::UnaryExpContext* SysYParser::UnaryExpContext::unaryExp() {
  return getRuleContext<SysYParser::UnaryExpContext>(0);
}

tree::TerminalNode* SysYParser::UnaryExpContext::ADD() {
  return getToken(SysYParser::ADD, 0);
}

tree::TerminalNode* SysYParser::UnaryExpContext::SUB() {
  return getToken(SysYParser::SUB, 0);
}

tree::TerminalNode* SysYParser::UnaryExpContext::LNOT() {
  return getToken(SysYParser::LNOT, 0);
}

tree::TerminalNode* SysYParser::UnaryExpContext::LP() {
  return getToken(SysYParser::LP, 0);
}

SysYParser::AdditiveExpContext* SysYParser::UnaryExpContext::additiveExp() {
  return getRuleContext<SysYParser::AdditiveExpContext>(0);
}

tree::TerminalNode* SysYParser::UnaryExpContext::RP() {
  return getToken(SysYParser::RP, 0);
}

SysYParser::VarExpContext* SysYParser::UnaryExpContext::varExp() {
  return getRuleContext<SysYParser::VarExpContext>(0);
}

SysYParser::FuncCallExpContext* SysYParser::UnaryExpContext::funcCallExp() {
  return getRuleContext<SysYParser::FuncCallExpContext>(0);
}

SysYParser::NumberExpContext* SysYParser::UnaryExpContext::numberExp() {
  return getRuleContext<SysYParser::NumberExpContext>(0);
}


size_t SysYParser::UnaryExpContext::getRuleIndex() const {
  return SysYParser::RuleUnaryExp;
}


std::any SysYParser::UnaryExpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitUnaryExp(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::UnaryExpContext* SysYParser::unaryExp() {
  UnaryExpContext *_localctx = _tracker.createInstance<UnaryExpContext>(_ctx, getState());
  enterRule(_localctx, 46, SysYParser::RuleUnaryExp);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(248);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 19, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(239);
      _la = _input->LA(1);
      if (!((((_la & ~ 0x3fULL) == 0) &&
        ((1ULL << _la) & 8400896) != 0))) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(240);
      unaryExp();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(241);
      match(SysYParser::LP);
      setState(242);
      additiveExp();
      setState(243);
      match(SysYParser::RP);
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(245);
      varExp();
      break;
    }

    case 4: {
      enterOuterAlt(_localctx, 4);
      setState(246);
      funcCallExp();
      break;
    }

    case 5: {
      enterOuterAlt(_localctx, 5);
      setState(247);
      numberExp();
      break;
    }

    default:
      break;
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- VarExpContext ------------------------------------------------------------------

SysYParser::VarExpContext::VarExpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SysYParser::ScalarVarExpContext* SysYParser::VarExpContext::scalarVarExp() {
  return getRuleContext<SysYParser::ScalarVarExpContext>(0);
}

SysYParser::ArrayVarExpContext* SysYParser::VarExpContext::arrayVarExp() {
  return getRuleContext<SysYParser::ArrayVarExpContext>(0);
}


size_t SysYParser::VarExpContext::getRuleIndex() const {
  return SysYParser::RuleVarExp;
}


std::any SysYParser::VarExpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitVarExp(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::VarExpContext* SysYParser::varExp() {
  VarExpContext *_localctx = _tracker.createInstance<VarExpContext>(_ctx, getState());
  enterRule(_localctx, 48, SysYParser::RuleVarExp);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(252);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 20, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(250);
      scalarVarExp();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(251);
      arrayVarExp();
      break;
    }

    default:
      break;
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ScalarVarExpContext ------------------------------------------------------------------

SysYParser::ScalarVarExpContext::ScalarVarExpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SysYParser::ScalarVarExpContext::Ident() {
  return getToken(SysYParser::Ident, 0);
}


size_t SysYParser::ScalarVarExpContext::getRuleIndex() const {
  return SysYParser::RuleScalarVarExp;
}


std::any SysYParser::ScalarVarExpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitScalarVarExp(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::ScalarVarExpContext* SysYParser::scalarVarExp() {
  ScalarVarExpContext *_localctx = _tracker.createInstance<ScalarVarExpContext>(_ctx, getState());
  enterRule(_localctx, 50, SysYParser::RuleScalarVarExp);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(254);
    match(SysYParser::Ident);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ArrayVarExpContext ------------------------------------------------------------------

SysYParser::ArrayVarExpContext::ArrayVarExpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SysYParser::ArrayVarExpContext::Ident() {
  return getToken(SysYParser::Ident, 0);
}

std::vector<tree::TerminalNode *> SysYParser::ArrayVarExpContext::LB() {
  return getTokens(SysYParser::LB);
}

tree::TerminalNode* SysYParser::ArrayVarExpContext::LB(size_t i) {
  return getToken(SysYParser::LB, i);
}

std::vector<SysYParser::AdditiveExpContext *> SysYParser::ArrayVarExpContext::additiveExp() {
  return getRuleContexts<SysYParser::AdditiveExpContext>();
}

SysYParser::AdditiveExpContext* SysYParser::ArrayVarExpContext::additiveExp(size_t i) {
  return getRuleContext<SysYParser::AdditiveExpContext>(i);
}

std::vector<tree::TerminalNode *> SysYParser::ArrayVarExpContext::RB() {
  return getTokens(SysYParser::RB);
}

tree::TerminalNode* SysYParser::ArrayVarExpContext::RB(size_t i) {
  return getToken(SysYParser::RB, i);
}


size_t SysYParser::ArrayVarExpContext::getRuleIndex() const {
  return SysYParser::RuleArrayVarExp;
}


std::any SysYParser::ArrayVarExpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitArrayVarExp(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::ArrayVarExpContext* SysYParser::arrayVarExp() {
  ArrayVarExpContext *_localctx = _tracker.createInstance<ArrayVarExpContext>(_ctx, getState());
  enterRule(_localctx, 52, SysYParser::RuleArrayVarExp);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(256);
    match(SysYParser::Ident);
    setState(261); 
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(257);
      match(SysYParser::LB);
      setState(258);
      additiveExp();
      setState(259);
      match(SysYParser::RB);
      setState(263); 
      _errHandler->sync(this);
      _la = _input->LA(1);
    } while (_la == SysYParser::LB);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- FuncCallExpContext ------------------------------------------------------------------

SysYParser::FuncCallExpContext::FuncCallExpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SysYParser::FuncCallExpContext::Ident() {
  return getToken(SysYParser::Ident, 0);
}

tree::TerminalNode* SysYParser::FuncCallExpContext::LP() {
  return getToken(SysYParser::LP, 0);
}

tree::TerminalNode* SysYParser::FuncCallExpContext::RP() {
  return getToken(SysYParser::RP, 0);
}

std::vector<SysYParser::AdditiveExpContext *> SysYParser::FuncCallExpContext::additiveExp() {
  return getRuleContexts<SysYParser::AdditiveExpContext>();
}

SysYParser::AdditiveExpContext* SysYParser::FuncCallExpContext::additiveExp(size_t i) {
  return getRuleContext<SysYParser::AdditiveExpContext>(i);
}

std::vector<tree::TerminalNode *> SysYParser::FuncCallExpContext::COMMA() {
  return getTokens(SysYParser::COMMA);
}

tree::TerminalNode* SysYParser::FuncCallExpContext::COMMA(size_t i) {
  return getToken(SysYParser::COMMA, i);
}


size_t SysYParser::FuncCallExpContext::getRuleIndex() const {
  return SysYParser::RuleFuncCallExp;
}


std::any SysYParser::FuncCallExpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitFuncCallExp(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::FuncCallExpContext* SysYParser::funcCallExp() {
  FuncCallExpContext *_localctx = _tracker.createInstance<FuncCallExpContext>(_ctx, getState());
  enterRule(_localctx, 54, SysYParser::RuleFuncCallExp);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(265);
    match(SysYParser::Ident);
    setState(266);
    match(SysYParser::LP);
    setState(275);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 120334594048) != 0)) {
      setState(267);
      additiveExp();
      setState(272);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == SysYParser::COMMA) {
        setState(268);
        match(SysYParser::COMMA);
        setState(269);
        additiveExp();
        setState(274);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
    }
    setState(277);
    match(SysYParser::RP);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- NumberExpContext ------------------------------------------------------------------

SysYParser::NumberExpContext::NumberExpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SysYParser::NumberExpContext::IntConst() {
  return getToken(SysYParser::IntConst, 0);
}

tree::TerminalNode* SysYParser::NumberExpContext::FloatConst() {
  return getToken(SysYParser::FloatConst, 0);
}


size_t SysYParser::NumberExpContext::getRuleIndex() const {
  return SysYParser::RuleNumberExp;
}


std::any SysYParser::NumberExpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitNumberExp(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::NumberExpContext* SysYParser::numberExp() {
  NumberExpContext *_localctx = _tracker.createInstance<NumberExpContext>(_ctx, getState());
  enterRule(_localctx, 56, SysYParser::RuleNumberExp);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(279);
    _la = _input->LA(1);
    if (!(_la == SysYParser::IntConst

    || _la == SysYParser::FloatConst)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- LorExpContext ------------------------------------------------------------------

SysYParser::LorExpContext::LorExpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<SysYParser::LandExpContext *> SysYParser::LorExpContext::landExp() {
  return getRuleContexts<SysYParser::LandExpContext>();
}

SysYParser::LandExpContext* SysYParser::LorExpContext::landExp(size_t i) {
  return getRuleContext<SysYParser::LandExpContext>(i);
}

std::vector<tree::TerminalNode *> SysYParser::LorExpContext::LOR() {
  return getTokens(SysYParser::LOR);
}

tree::TerminalNode* SysYParser::LorExpContext::LOR(size_t i) {
  return getToken(SysYParser::LOR, i);
}


size_t SysYParser::LorExpContext::getRuleIndex() const {
  return SysYParser::RuleLorExp;
}


std::any SysYParser::LorExpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitLorExp(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::LorExpContext* SysYParser::lorExp() {
  LorExpContext *_localctx = _tracker.createInstance<LorExpContext>(_ctx, getState());
  enterRule(_localctx, 58, SysYParser::RuleLorExp);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(281);
    landExp();
    setState(286);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == SysYParser::LOR) {
      setState(282);
      match(SysYParser::LOR);
      setState(283);
      landExp();
      setState(288);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- LandExpContext ------------------------------------------------------------------

SysYParser::LandExpContext::LandExpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<SysYParser::EqualityExpContext *> SysYParser::LandExpContext::equalityExp() {
  return getRuleContexts<SysYParser::EqualityExpContext>();
}

SysYParser::EqualityExpContext* SysYParser::LandExpContext::equalityExp(size_t i) {
  return getRuleContext<SysYParser::EqualityExpContext>(i);
}

std::vector<tree::TerminalNode *> SysYParser::LandExpContext::LAND() {
  return getTokens(SysYParser::LAND);
}

tree::TerminalNode* SysYParser::LandExpContext::LAND(size_t i) {
  return getToken(SysYParser::LAND, i);
}


size_t SysYParser::LandExpContext::getRuleIndex() const {
  return SysYParser::RuleLandExp;
}


std::any SysYParser::LandExpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitLandExp(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::LandExpContext* SysYParser::landExp() {
  LandExpContext *_localctx = _tracker.createInstance<LandExpContext>(_ctx, getState());
  enterRule(_localctx, 60, SysYParser::RuleLandExp);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(289);
    equalityExp();
    setState(294);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == SysYParser::LAND) {
      setState(290);
      match(SysYParser::LAND);
      setState(291);
      equalityExp();
      setState(296);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- EqualityExpContext ------------------------------------------------------------------

SysYParser::EqualityExpContext::EqualityExpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<SysYParser::RelationalExpContext *> SysYParser::EqualityExpContext::relationalExp() {
  return getRuleContexts<SysYParser::RelationalExpContext>();
}

SysYParser::RelationalExpContext* SysYParser::EqualityExpContext::relationalExp(size_t i) {
  return getRuleContext<SysYParser::RelationalExpContext>(i);
}

std::vector<tree::TerminalNode *> SysYParser::EqualityExpContext::EQ() {
  return getTokens(SysYParser::EQ);
}

tree::TerminalNode* SysYParser::EqualityExpContext::EQ(size_t i) {
  return getToken(SysYParser::EQ, i);
}

std::vector<tree::TerminalNode *> SysYParser::EqualityExpContext::NE() {
  return getTokens(SysYParser::NE);
}

tree::TerminalNode* SysYParser::EqualityExpContext::NE(size_t i) {
  return getToken(SysYParser::NE, i);
}


size_t SysYParser::EqualityExpContext::getRuleIndex() const {
  return SysYParser::RuleEqualityExp;
}


std::any SysYParser::EqualityExpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitEqualityExp(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::EqualityExpContext* SysYParser::equalityExp() {
  EqualityExpContext *_localctx = _tracker.createInstance<EqualityExpContext>(_ctx, getState());
  enterRule(_localctx, 62, SysYParser::RuleEqualityExp);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(297);
    relationalExp();
    setState(302);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == SysYParser::EQ

    || _la == SysYParser::NE) {
      setState(298);
      _la = _input->LA(1);
      if (!(_la == SysYParser::EQ

      || _la == SysYParser::NE)) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(299);
      relationalExp();
      setState(304);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- RelationalExpContext ------------------------------------------------------------------

SysYParser::RelationalExpContext::RelationalExpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<SysYParser::AdditiveExpContext *> SysYParser::RelationalExpContext::additiveExp() {
  return getRuleContexts<SysYParser::AdditiveExpContext>();
}

SysYParser::AdditiveExpContext* SysYParser::RelationalExpContext::additiveExp(size_t i) {
  return getRuleContext<SysYParser::AdditiveExpContext>(i);
}

std::vector<tree::TerminalNode *> SysYParser::RelationalExpContext::LT() {
  return getTokens(SysYParser::LT);
}

tree::TerminalNode* SysYParser::RelationalExpContext::LT(size_t i) {
  return getToken(SysYParser::LT, i);
}

std::vector<tree::TerminalNode *> SysYParser::RelationalExpContext::GT() {
  return getTokens(SysYParser::GT);
}

tree::TerminalNode* SysYParser::RelationalExpContext::GT(size_t i) {
  return getToken(SysYParser::GT, i);
}

std::vector<tree::TerminalNode *> SysYParser::RelationalExpContext::LE() {
  return getTokens(SysYParser::LE);
}

tree::TerminalNode* SysYParser::RelationalExpContext::LE(size_t i) {
  return getToken(SysYParser::LE, i);
}

std::vector<tree::TerminalNode *> SysYParser::RelationalExpContext::GE() {
  return getTokens(SysYParser::GE);
}

tree::TerminalNode* SysYParser::RelationalExpContext::GE(size_t i) {
  return getToken(SysYParser::GE, i);
}


size_t SysYParser::RelationalExpContext::getRuleIndex() const {
  return SysYParser::RuleRelationalExp;
}


std::any SysYParser::RelationalExpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitRelationalExp(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::RelationalExpContext* SysYParser::relationalExp() {
  RelationalExpContext *_localctx = _tracker.createInstance<RelationalExpContext>(_ctx, getState());
  enterRule(_localctx, 64, SysYParser::RuleRelationalExp);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(305);
    additiveExp();
    setState(310);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 7864320) != 0)) {
      setState(306);
      _la = _input->LA(1);
      if (!((((_la & ~ 0x3fULL) == 0) &&
        ((1ULL << _la) & 7864320) != 0))) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(307);
      additiveExp();
      setState(312);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- AdditiveExpContext ------------------------------------------------------------------

SysYParser::AdditiveExpContext::AdditiveExpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<SysYParser::MultiplicativeExpContext *> SysYParser::AdditiveExpContext::multiplicativeExp() {
  return getRuleContexts<SysYParser::MultiplicativeExpContext>();
}

SysYParser::MultiplicativeExpContext* SysYParser::AdditiveExpContext::multiplicativeExp(size_t i) {
  return getRuleContext<SysYParser::MultiplicativeExpContext>(i);
}

std::vector<tree::TerminalNode *> SysYParser::AdditiveExpContext::ADD() {
  return getTokens(SysYParser::ADD);
}

tree::TerminalNode* SysYParser::AdditiveExpContext::ADD(size_t i) {
  return getToken(SysYParser::ADD, i);
}

std::vector<tree::TerminalNode *> SysYParser::AdditiveExpContext::SUB() {
  return getTokens(SysYParser::SUB);
}

tree::TerminalNode* SysYParser::AdditiveExpContext::SUB(size_t i) {
  return getToken(SysYParser::SUB, i);
}


size_t SysYParser::AdditiveExpContext::getRuleIndex() const {
  return SysYParser::RuleAdditiveExp;
}


std::any SysYParser::AdditiveExpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitAdditiveExp(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::AdditiveExpContext* SysYParser::additiveExp() {
  AdditiveExpContext *_localctx = _tracker.createInstance<AdditiveExpContext>(_ctx, getState());
  enterRule(_localctx, 66, SysYParser::RuleAdditiveExp);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(313);
    multiplicativeExp();
    setState(318);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == SysYParser::ADD

    || _la == SysYParser::SUB) {
      setState(314);
      _la = _input->LA(1);
      if (!(_la == SysYParser::ADD

      || _la == SysYParser::SUB)) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(315);
      multiplicativeExp();
      setState(320);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- MultiplicativeExpContext ------------------------------------------------------------------

SysYParser::MultiplicativeExpContext::MultiplicativeExpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<SysYParser::UnaryExpContext *> SysYParser::MultiplicativeExpContext::unaryExp() {
  return getRuleContexts<SysYParser::UnaryExpContext>();
}

SysYParser::UnaryExpContext* SysYParser::MultiplicativeExpContext::unaryExp(size_t i) {
  return getRuleContext<SysYParser::UnaryExpContext>(i);
}

std::vector<tree::TerminalNode *> SysYParser::MultiplicativeExpContext::MUL() {
  return getTokens(SysYParser::MUL);
}

tree::TerminalNode* SysYParser::MultiplicativeExpContext::MUL(size_t i) {
  return getToken(SysYParser::MUL, i);
}

std::vector<tree::TerminalNode *> SysYParser::MultiplicativeExpContext::DIV() {
  return getTokens(SysYParser::DIV);
}

tree::TerminalNode* SysYParser::MultiplicativeExpContext::DIV(size_t i) {
  return getToken(SysYParser::DIV, i);
}

std::vector<tree::TerminalNode *> SysYParser::MultiplicativeExpContext::MOD() {
  return getTokens(SysYParser::MOD);
}

tree::TerminalNode* SysYParser::MultiplicativeExpContext::MOD(size_t i) {
  return getToken(SysYParser::MOD, i);
}


size_t SysYParser::MultiplicativeExpContext::getRuleIndex() const {
  return SysYParser::RuleMultiplicativeExp;
}


std::any SysYParser::MultiplicativeExpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYVisitor*>(visitor))
    return parserVisitor->visitMultiplicativeExp(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::MultiplicativeExpContext* SysYParser::multiplicativeExp() {
  MultiplicativeExpContext *_localctx = _tracker.createInstance<MultiplicativeExpContext>(_ctx, getState());
  enterRule(_localctx, 68, SysYParser::RuleMultiplicativeExp);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(321);
    unaryExp();
    setState(326);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 114688) != 0)) {
      setState(322);
      _la = _input->LA(1);
      if (!((((_la & ~ 0x3fULL) == 0) &&
        ((1ULL << _la) & 114688) != 0))) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(323);
      unaryExp();
      setState(328);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

void SysYParser::initialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  sysyParserInitialize();
#else
  ::antlr4::internal::call_once(sysyParserOnceFlag, sysyParserInitialize);
#endif
}
