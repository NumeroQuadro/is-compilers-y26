#include "CalculatorInterpreter.h"

int CalculatorInterpreter::get_result() {
  return int_stack.top();
}

antlrcpp::Any CalculatorInterpreter::visitPlusOp(GrammarParser::PlusOpContext *ctx) {
  visit(ctx->children[0]);
  visit(ctx->children[2]);
  auto r = int_stack.top(); int_stack.pop();
  auto l = int_stack.top(); int_stack.pop();
  int_stack.push(l + r);
  return nullptr;
}

antlrcpp::Any CalculatorInterpreter::visitBinaryMinusOp(GrammarParser::BinaryMinusOpContext *ctx) {
  visit(ctx->children[0]);
  visit(ctx->children[2]);
  auto r = int_stack.top(); int_stack.pop();
  auto l = int_stack.top(); int_stack.pop();
  int_stack.push(l - r);
  return nullptr;
}

antlrcpp::Any CalculatorInterpreter::visitUnaryMinusOp(GrammarParser::UnaryMinusOpContext *ctx) {
  visit(ctx->children[1]);
  int_stack.top() = -int_stack.top();
  return nullptr;
}

antlrcpp::Any CalculatorInterpreter::visitMultOp(GrammarParser::MultOpContext *ctx) {
  visit(ctx->children[0]);
  visit(ctx->children[2]);
  auto r = int_stack.top(); int_stack.pop();
  auto l = int_stack.top(); int_stack.pop();
  int_stack.push(l * r);
  return nullptr;
}

antlrcpp::Any CalculatorInterpreter::visitDivOp(GrammarParser::DivOpContext *ctx) {
  visit(ctx->children[0]);
  visit(ctx->children[2]);
  auto r = int_stack.top(); int_stack.pop();
  auto l = int_stack.top(); int_stack.pop();
  int_stack.push(l / r);
  return nullptr;
}

antlrcpp::Any CalculatorInterpreter::visitInt(GrammarParser::IntContext *ctx) {
  int value = std::stoi(ctx->getText());
  int_stack.push(value);
  return nullptr;
}

// Реализация visitIfElseStatement
antlrcpp::Any CalculatorInterpreter::visitIfElseStatement(GrammarParser::IfElseStatementContext *ctx) {
  // Посещаем условие
  visit(ctx->condition());
  int conditionResult = int_stack.top();
  int_stack.pop();

  if (conditionResult) {
    // Посещаем блок для 'if'
    visit(ctx->block(0));
  } else {
    // Посещаем блок для 'else'
    visit(ctx->block(1));
  }
  return nullptr;
}

antlrcpp::Any CalculatorInterpreter::visitGreaterThanCondition(GrammarParser::GreaterThanConditionContext *ctx) {
  // Посещаем левый операнд
  visit(ctx->plusOrMinus(0));
  int left = int_stack.top();
  int_stack.pop();

  // Посещаем правый операнд
  visit(ctx->plusOrMinus(1));
  int right = int_stack.top();
  int_stack.pop();

  // Сравниваем и помещаем результат в стек
  int_stack.push(left > right ? 1 : 0);
  return nullptr;
}

antlrcpp::Any CalculatorInterpreter::visitCodeBlock(GrammarParser::CodeBlockContext *ctx) {
  // Посещаем все statements внутри блока
  for (auto statementCtx : ctx->statement()) {
    visit(statementCtx);
  }
  return nullptr;
}