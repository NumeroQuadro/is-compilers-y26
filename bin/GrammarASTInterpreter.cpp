#include "GrammarASTInterpreter.h"

#include "Instruction.h"

antlrcpp::Any GrammarASTInterpreter::visitScript(GrammarParser::ScriptContext *ctx) {
  for (const auto v: ctx->vardef()) {
    visit(v);
  }
  for (auto f: ctx->function()) {
    // no functions
  }
  for (const auto s: ctx->statement()) {
    visit(s);
  }
  code.emplace_back(InstructionType::HALT);
  return nullptr;
}

antlrcpp::Any GrammarASTInterpreter::visitVardef(GrammarParser::VardefContext *ctx) {
  // var ID = expr
  visit(ctx->expr());
  code.emplace_back(InstructionType::STORE_VAR, ctx->ID()->getText());
  return nullptr;
}

antlrcpp::Any GrammarASTInterpreter::visitIf(GrammarParser::IfContext *ctx) {
  visit(ctx->expr());
  const int jmzIndex = static_cast<int>(code.size());
  code.emplace_back(InstructionType::JMZ, 0);

  visit(ctx->statement(0));
  const int jmpIndex = static_cast<int>(code.size());
  code.emplace_back(InstructionType::JMP, 0);

  const int elseStart = static_cast<int>(code.size());
  if (ctx->statement().size() > 1) {
    visit(ctx->statement(1));
  }
  const int endPos = static_cast<int>(code.size());

  code[jmzIndex].intOperand = elseStart;
  code[jmpIndex].intOperand = endPos;
  return nullptr;
}

antlrcpp::Any GrammarASTInterpreter::visitAssign(GrammarParser::AssignContext *ctx) {
  visit(ctx->expr());
  code.emplace_back(InstructionType::STORE_VAR, ctx->ID()->getText());
  return nullptr;
}

antlrcpp::Any GrammarASTInterpreter::visitPrint(GrammarParser::PrintContext *ctx) {
  if (ctx->expr()) {
    visit(ctx->expr());
  }
  code.emplace_back(InstructionType::PRINT);
  return nullptr;
}

antlrcpp::Any GrammarASTInterpreter::visitOp(GrammarParser::OpContext *ctx) {
  visit(ctx->expr(0));
  visit(ctx->expr(1));
  std::string op = ctx->operator_()->getText();
  if (op == "+") code.emplace_back(InstructionType::ADD);
  else if (op == "-") code.emplace_back(InstructionType::SUB);
  else if (op == "*") code.emplace_back(InstructionType::MUL);
  else if (op == "/") code.emplace_back(InstructionType::DIV);
  else if (op == "==") code.emplace_back(InstructionType::EQ);
  else if (op == "!=") code.emplace_back(InstructionType::NEQ);
  else if (op == "<") code.emplace_back(InstructionType::LT);
  else if (op == "<=") code.emplace_back(InstructionType::LE);
  else if (op == ">") code.emplace_back(InstructionType::GT);
  else if (op == ">=") code.emplace_back(InstructionType::GE);
  return nullptr;
}

antlrcpp::Any GrammarASTInterpreter::visitNegate(GrammarParser::NegateContext *ctx) {
  visit(ctx->expr());
  code.emplace_back(InstructionType::NEG);
  return nullptr;
}

antlrcpp::Any GrammarASTInterpreter::visitNot(GrammarParser::NotContext *ctx) {
  visit(ctx->expr());
  code.emplace_back(InstructionType::NOT);
  return nullptr;
}

antlrcpp::Any GrammarASTInterpreter::visitParens(GrammarParser::ParensContext *ctx) {
  return visit(ctx->expr());
}

antlrcpp::Any GrammarASTInterpreter::visitAtom(GrammarParser::AtomContext *ctx) {
  return visit(ctx->primary());
}

antlrcpp::Any GrammarASTInterpreter::visitIdentifier(GrammarParser::IdentifierContext *ctx) {
  code.emplace_back(InstructionType::PUSH_VAR, ctx->ID()->getText());
  return nullptr;
}

antlrcpp::Any GrammarASTInterpreter::visitInteger(GrammarParser::IntegerContext *ctx) {
  int val = std::stoi(ctx->INT()->getText());
  code.emplace_back(InstructionType::PUSH_INT, val);
  return nullptr;
}

antlrcpp::Any GrammarASTInterpreter::visitString(GrammarParser::StringContext *ctx) {
  std::string str = ctx->STRING()->getText();
  str = str.substr(1, str.size() - 2);
  code.emplace_back(InstructionType::PUSH_STRING, str);
  return nullptr;
}

antlrcpp::Any GrammarASTInterpreter::visitTrueLiteral(GrammarParser::TrueLiteralContext *ctx) {
  code.emplace_back(InstructionType::PUSH_BOOL, true);
  return nullptr;
}

antlrcpp::Any GrammarASTInterpreter::visitFalseLiteral(GrammarParser::FalseLiteralContext *ctx) {
  code.emplace_back(InstructionType::PUSH_BOOL, false);
  return nullptr;
}

antlrcpp::Any GrammarASTInterpreter::visitVector(GrammarParser::VectorContext *ctx) {
  int count = static_cast<int>(ctx->expr_list()->expr().size());
  code.emplace_back(InstructionType::NEW_ARRAY, count);
  for (int i = 0; i < count; i++) {
    code.emplace_back(InstructionType::PUSH_VAR, ""); // тут tricky: нет arr в var, значит надо дублировать верх стека
    visit(ctx->expr_list()->expr(i));
    code.emplace_back(InstructionType::PUSH_INT, i);
    code.emplace_back(InstructionType::PUSH_INT, i);
  }

  return nullptr;
}

antlrcpp::Any GrammarASTInterpreter::visitIndex(GrammarParser::IndexContext *ctx) {
  code.emplace_back(InstructionType::PUSH_VAR, ctx->ID()->getText());
  visit(ctx->expr());
  code.emplace_back(InstructionType::GET_ELEMENT);
  return nullptr;
}

antlrcpp::Any GrammarASTInterpreter::visitElementAssign(GrammarParser::ElementAssignContext *ctx) {
  code.emplace_back(InstructionType::PUSH_VAR, ctx->ID()->getText());
  visit(ctx->expr(0));
  visit(ctx->expr(1));
  code.emplace_back(InstructionType::SET_ELEMENT);
  return nullptr;
}
