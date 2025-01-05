#pragma once

#include <fstream>

#include "antlr4-runtime.h"
#include "FunctionInfo.h"
#include "GrammarBaseVisitor.h"
#include "GrammarParser.h"
#include "Instruction.h"

class GrammarASTInterpreter final : public GrammarBaseVisitor {
public:
    std::vector<Instruction> code;
    std::unordered_map<std::string, FunctionInfo> functionTable;
    size_t startPos = 0;

    void toFile(const std::string &path);
private:

    antlrcpp::Any visitScript(GrammarParser::ScriptContext *ctx) override;

    antlrcpp::Any visitBlockStatement(GrammarParser::BlockStatementContext *ctx) override;

    antlrcpp::Any visitBlock(GrammarParser::BlockContext *ctx) override;

    antlrcpp::Any visitIf(GrammarParser::IfContext *ctx) override;

    antlrcpp::Any visitWhile(GrammarParser::WhileContext *ctx) override;

    antlrcpp::Any visitVardef(GrammarParser::VardefContext *ctx) override;

    antlrcpp::Any visitAssign(GrammarParser::AssignContext *ctx) override;

    antlrcpp::Any visitPrint(GrammarParser::PrintContext *ctx) override;

    antlrcpp::Any visitFor(GrammarParser::ForContext *ctx) override;

    antlrcpp::Any visitForInit(GrammarParser::ForInitContext *ctx) override;

    antlrcpp::Any visitForIncrement(GrammarParser::ForIncrementContext *ctx) override;

    antlrcpp::Any visitLogicalOrExpr(GrammarParser::LogicalOrExprContext *ctx) override;

    antlrcpp::Any visitLogicalAndExpr(GrammarParser::LogicalAndExprContext *ctx) override;

    antlrcpp::Any visitEqualityExpr(GrammarParser::EqualityExprContext *ctx) override;

    antlrcpp::Any visitRelationalExpr(GrammarParser::RelationalExprContext *ctx) override;

    antlrcpp::Any visitAdditiveExpr(GrammarParser::AdditiveExprContext *ctx) override;

    antlrcpp::Any visitMultiplicativeExpr(GrammarParser::MultiplicativeExprContext *ctx) override;

    antlrcpp::Any visitUnaryExpr(GrammarParser::UnaryExprContext *ctx) override;

    antlrcpp::Any visitParens(GrammarParser::ParensContext *ctx) override;

    antlrcpp::Any visitAtom(GrammarParser::AtomContext *ctx) override;

    antlrcpp::Any visitIdentifier(GrammarParser::IdentifierContext *ctx) override;

    antlrcpp::Any visitInteger(GrammarParser::IntegerContext *ctx) override;

    antlrcpp::Any visitFloat(GrammarParser::FloatContext *ctx) override;

    antlrcpp::Any visitString(GrammarParser::StringContext *ctx) override;

    antlrcpp::Any visitTrueLiteral(GrammarParser::TrueLiteralContext *ctx) override;

    antlrcpp::Any visitFalseLiteral(GrammarParser::FalseLiteralContext *ctx) override;

    antlrcpp::Any visitVector(GrammarParser::VectorContext *ctx) override;

    antlrcpp::Any visitIndex(GrammarParser::IndexContext *ctx) override;

    antlrcpp::Any visitArraySize(GrammarParser::ArraySizeContext *ctx) override;

    antlrcpp::Any visitMethodCall(GrammarParser::MethodCallContext *ctx) override;

    antlrcpp::Any visitElementAssign(GrammarParser::ElementAssignContext *ctx) override;

    antlrcpp::Any visitFunction(GrammarParser::FunctionContext *ctx) override;

    antlrcpp::Any visitReturn(GrammarParser::ReturnContext *context) override;

    antlrcpp::Any visitCallStatement(GrammarParser::CallStatementContext *ctx) override;

    antlrcpp::Any visitCall_expr(GrammarParser::Call_exprContext *ctx) override;


};
