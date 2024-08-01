
// Generated from SysY.g4 by ANTLR 4.13.1

#pragma once


#include "antlr4-runtime.h"
#include "SysYParser.h"



/**
 * This class defines an abstract visitor for a parse tree
 * produced by SysYParser.
 */
class  SysYVisitor : public antlr4::tree::AbstractParseTreeVisitor {
public:

  /**
   * Visit parse trees produced by SysYParser.
   */
    virtual std::any visitRoot(SysYParser::RootContext *context) = 0;

    virtual std::any visitCompUnit(SysYParser::CompUnitContext *context) = 0;

    virtual std::any visitType(SysYParser::TypeContext *context) = 0;

    virtual std::any visitDimensions(SysYParser::DimensionsContext *context) = 0;

    virtual std::any visitVarDecl(SysYParser::VarDeclContext *context) = 0;

    virtual std::any visitVarDef(SysYParser::VarDefContext *context) = 0;

    virtual std::any visitScalarVarDef(SysYParser::ScalarVarDefContext *context) = 0;

    virtual std::any visitArrayVarDef(SysYParser::ArrayVarDefContext *context) = 0;

    virtual std::any visitInitVal(SysYParser::InitValContext *context) = 0;

    virtual std::any visitFuncDef(SysYParser::FuncDefContext *context) = 0;

    virtual std::any visitFuncArg(SysYParser::FuncArgContext *context) = 0;

    virtual std::any visitBlockStmt(SysYParser::BlockStmtContext *context) = 0;

    virtual std::any visitStmt(SysYParser::StmtContext *context) = 0;

    virtual std::any visitAssignStmt(SysYParser::AssignStmtContext *context) = 0;

    virtual std::any visitBlankStmt(SysYParser::BlankStmtContext *context) = 0;

    virtual std::any visitExpStmt(SysYParser::ExpStmtContext *context) = 0;

    virtual std::any visitIfElseStmt(SysYParser::IfElseStmtContext *context) = 0;

    virtual std::any visitIfStmt(SysYParser::IfStmtContext *context) = 0;

    virtual std::any visitWhileStmt(SysYParser::WhileStmtContext *context) = 0;

    virtual std::any visitBreakStmt(SysYParser::BreakStmtContext *context) = 0;

    virtual std::any visitContinueStmt(SysYParser::ContinueStmtContext *context) = 0;

    virtual std::any visitRetStmt(SysYParser::RetStmtContext *context) = 0;

    virtual std::any visitLVal(SysYParser::LValContext *context) = 0;

    virtual std::any visitUnaryExp(SysYParser::UnaryExpContext *context) = 0;

    virtual std::any visitVarExp(SysYParser::VarExpContext *context) = 0;

    virtual std::any visitScalarVarExp(SysYParser::ScalarVarExpContext *context) = 0;

    virtual std::any visitArrayVarExp(SysYParser::ArrayVarExpContext *context) = 0;

    virtual std::any visitFuncCallExp(SysYParser::FuncCallExpContext *context) = 0;

    virtual std::any visitNumberExp(SysYParser::NumberExpContext *context) = 0;

    virtual std::any visitLorExp(SysYParser::LorExpContext *context) = 0;

    virtual std::any visitLandExp(SysYParser::LandExpContext *context) = 0;

    virtual std::any visitEqualityExp(SysYParser::EqualityExpContext *context) = 0;

    virtual std::any visitRelationalExp(SysYParser::RelationalExpContext *context) = 0;

    virtual std::any visitAdditiveExp(SysYParser::AdditiveExpContext *context) = 0;

    virtual std::any visitMultiplicativeExp(SysYParser::MultiplicativeExpContext *context) = 0;


};

