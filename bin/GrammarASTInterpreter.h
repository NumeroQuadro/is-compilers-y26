#pragma once
#include "antlr4-runtime.h"
#include "GrammarBaseVisitor.h"
#include "GrammarParser.h"
#include "Value.h"

struct Instruction;

class GrammarASTInterpreter final : public GrammarBaseVisitor {
public:
  std::vector<Instruction> code;
private:
  antlrcpp::Any visitScript(GrammarParser::ScriptContext *ctx) override;
  antlrcpp::Any visitVardef(GrammarParser::VardefContext *ctx) override;
  antlrcpp::Any visitIf(GrammarParser::IfContext *ctx) override;
  antlrcpp::Any visitAssign(GrammarParser::AssignContext *ctx) override;
  antlrcpp::Any visitPrint(GrammarParser::PrintContext *ctx) override;

  antlrcpp::Any visitOp(GrammarParser::OpContext *ctx) override;
  antlrcpp::Any visitNegate(GrammarParser::NegateContext *ctx) override;
  antlrcpp::Any visitNot(GrammarParser::NotContext *ctx) override;
  antlrcpp::Any visitParens(GrammarParser::ParensContext *ctx) override;
  antlrcpp::Any visitAtom(GrammarParser::AtomContext *ctx) override;
  antlrcpp::Any visitIdentifier(GrammarParser::IdentifierContext *ctx) override;
  antlrcpp::Any visitInteger(GrammarParser::IntegerContext *ctx) override;
  antlrcpp::Any visitString(GrammarParser::StringContext *ctx) override;
  antlrcpp::Any visitTrueLiteral(GrammarParser::TrueLiteralContext *ctx) override;
  antlrcpp::Any visitFalseLiteral(GrammarParser::FalseLiteralContext *ctx) override;

  antlrcpp::Any visitVector(GrammarParser::VectorContext *ctx) override;
  antlrcpp::Any visitIndex(GrammarParser::IndexContext *ctx) override;
  antlrcpp::Any visitElementAssign(GrammarParser::ElementAssignContext *ctx) override;
};
