#pragma once

#include <GrammarBaseVisitor.h>
#include <stack>

/*
 * interpet an aritmetic expression on integer
 * supported operation: +,-,*,/
 */
class CalculatorInterpreter : public GrammarBaseVisitor {

  // Results stack
  std::stack<int> int_stack;

public:
  int get_result();

  antlrcpp::Any visitPlusOp(GrammarParser::PlusOpContext *ctx) override;
  antlrcpp::Any visitBinaryMinusOp(GrammarParser::BinaryMinusOpContext *ctx) override;
  antlrcpp::Any visitUnaryMinusOp(GrammarParser::UnaryMinusOpContext *ctx) override;
  antlrcpp::Any visitMultOp(GrammarParser::MultOpContext *ctx) override;
  antlrcpp::Any visitDivOp(GrammarParser::DivOpContext *ctx) override;
  antlrcpp::Any visitInt(GrammarParser::IntContext *ctx) override;
  antlrcpp::Any visitIfElseStatement(GrammarParser::IfElseStatementContext *ctx) override;
  antlrcpp::Any visitGreaterThanCondition(GrammarParser::GreaterThanConditionContext *ctx) override;
  antlrcpp::Any visitCodeBlock(GrammarParser::CodeBlockContext *ctx) override;
};