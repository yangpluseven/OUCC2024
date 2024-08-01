
// Generated from SysY.g4 by ANTLR 4.13.1

#pragma once


#include "antlr4-runtime.h"
#include "SysYVisitor.h"


/**
 * This class provides an empty implementation of SysYVisitor, which can be
 * extended to create a visitor which only needs to handle a subset of the available methods.
 */
class  SysYBaseVisitor : public SysYVisitor {
public:

  virtual std::any visitRoot(SysYParser::RootContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCompUnit(SysYParser::CompUnitContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitType(SysYParser::TypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDimensions(SysYParser::DimensionsContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitVarDecl(SysYParser::VarDeclContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitVarDef(SysYParser::VarDefContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitScalarVarDef(SysYParser::ScalarVarDefContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitArrayVarDef(SysYParser::ArrayVarDefContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitInitVal(SysYParser::InitValContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFuncDef(SysYParser::FuncDefContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFuncArg(SysYParser::FuncArgContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBlockStmt(SysYParser::BlockStmtContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStmt(SysYParser::StmtContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAssignStmt(SysYParser::AssignStmtContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBlankStmt(SysYParser::BlankStmtContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExpStmt(SysYParser::ExpStmtContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitIfElseStmt(SysYParser::IfElseStmtContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitIfStmt(SysYParser::IfStmtContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitWhileStmt(SysYParser::WhileStmtContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBreakStmt(SysYParser::BreakStmtContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitContinueStmt(SysYParser::ContinueStmtContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitRetStmt(SysYParser::RetStmtContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLVal(SysYParser::LValContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitUnaryExp(SysYParser::UnaryExpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitVarExp(SysYParser::VarExpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitScalarVarExp(SysYParser::ScalarVarExpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitArrayVarExp(SysYParser::ArrayVarExpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFuncCallExp(SysYParser::FuncCallExpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNumberExp(SysYParser::NumberExpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLorExp(SysYParser::LorExpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLandExp(SysYParser::LandExpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitEqualityExp(SysYParser::EqualityExpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitRelationalExp(SysYParser::RelationalExpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAdditiveExp(SysYParser::AdditiveExpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitMultiplicativeExp(SysYParser::MultiplicativeExpContext *ctx) override {
    return visitChildren(ctx);
  }


};

