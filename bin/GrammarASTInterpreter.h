#include "antlr4-runtime.h"
#include "GrammarBaseVisitor.h"
#include "GrammarParser.h"
#include "Value.h"

class GrammarASTInterpreter : public GrammarBaseVisitor {
public:
  std::map<std::string, Value> globalScope;
  std::map<std::string, GrammarParser::FunctionContext *> functionMap;

  antlrcpp::Any visitScript(GrammarParser::ScriptContext *context) override;

  antlrcpp::Any visitFunction(GrammarParser::FunctionContext *context) override;

  antlrcpp::Any visitBlockStatement(GrammarParser::BlockStatementContext *context) override;

  antlrcpp::Any visitBlock(GrammarParser::BlockContext *context) override;

  antlrcpp::Any visitReturn(GrammarParser::ReturnContext *context) override;

  antlrcpp::Any visitCallStatement(GrammarParser::CallStatementContext *context) override;

  antlrcpp::Any visitCall(GrammarParser::CallContext *context) override;

  antlrcpp::Any visitCall_expr(GrammarParser::Call_exprContext *context) override;

  antlrcpp::Any visitVardef(GrammarParser::VardefContext *context) override;

  antlrcpp::Any visitIf(GrammarParser::IfContext *context) override;

  antlrcpp::Any visitWhile(GrammarParser::WhileContext *context) override;

  antlrcpp::Any visitAssign(GrammarParser::AssignContext *context) override;

  antlrcpp::Any visitElementAssign(GrammarParser::ElementAssignContext *context) override;

  antlrcpp::Any visitPrint(GrammarParser::PrintContext *context) override;

  antlrcpp::Any visitOp(GrammarParser::OpContext *context) override;

  antlrcpp::Any visitNegate(GrammarParser::NegateContext *context) override;

  antlrcpp::Any visitNot(GrammarParser::NotContext *context) override;

  antlrcpp::Any visitParens(GrammarParser::ParensContext *context) override;

  antlrcpp::Any visitAtom(GrammarParser::AtomContext *context) override;

  antlrcpp::Any visitIdentifier(GrammarParser::IdentifierContext *context) override;

  antlrcpp::Any visitInteger(GrammarParser::IntegerContext *context) override;

  antlrcpp::Any visitFloat(GrammarParser::FloatContext *context) override;

  antlrcpp::Any visitString(GrammarParser::StringContext *context) override;

  antlrcpp::Any visitVector(GrammarParser::VectorContext *context) override;

  antlrcpp::Any visitTrueLiteral(GrammarParser::TrueLiteralContext *context) override;

  antlrcpp::Any visitFalseLiteral(GrammarParser::FalseLiteralContext *context) override;

private:
  Value evaluate(antlr4::tree::ParseTree *node);

  Value callFunction(const std::string &functionName, const std::vector<Value> &args);

  Value &getVariable(const std::string &variableName);

  bool isTruthy(const Value &value);
};
