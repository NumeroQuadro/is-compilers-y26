#include "GrammarASTInterpreter.h"
#include "GrammarParser.h"
#include "Instruction.h"

antlrcpp::Any GrammarASTInterpreter::visitScript(GrammarParser::ScriptContext *ctx) {
  for (const auto v: ctx->vardef()) {
    visit(v);
  }

  for (const auto f: ctx->function()) {
    visit(f);
  }
  startPos = code.size();

  for (const auto s: ctx->statement()) {
    visit(s);
  }

  code.emplace_back(InstructionType::HALT);
  return nullptr;
}

antlrcpp::Any GrammarASTInterpreter::visitBlock(GrammarParser::BlockContext *ctx) {
  for (auto c: ctx->children) {
    auto stmt = dynamic_cast<GrammarParser::StatementContext *>(c);
    if (stmt) visit(stmt);
    auto vd = dynamic_cast<GrammarParser::VardefContext *>(c);
    if (vd) visit(vd);
  }
  return nullptr;
}

antlrcpp::Any GrammarASTInterpreter::visitBlockStatement(GrammarParser::BlockStatementContext *ctx) {
  code.emplace_back(InstructionType::ENTER_SCOPE);
  visit(ctx->block());
  code.emplace_back(InstructionType::EXIT_SCOPE);
  return nullptr;
}

antlrcpp::Any GrammarASTInterpreter::visitIf(GrammarParser::IfContext *ctx) {
  visit(ctx->expr());
  const size_t jmzIndex = code.size();
  code.emplace_back(InstructionType::JMZ, static_cast<int64_t>(0));

  visit(ctx->statement(0));
  const size_t jmpIndex = code.size();
  code.emplace_back(InstructionType::JMP, static_cast<int64_t>(0));

  const size_t elseStart = code.size();
  if (ctx->statement().size() > 1) {
    visit(ctx->statement(1));
  }
  const size_t endPos = code.size();

  code[jmzIndex].intOperand = elseStart;
  code[jmpIndex].intOperand = endPos;
  return nullptr;
}

antlrcpp::Any GrammarASTInterpreter::visitWhile(GrammarParser::WhileContext *ctx) {
  const int64_t startIp = code.size();
  visit(ctx->expr());
  const int64_t jmzIndex = code.size();
  code.emplace_back(InstructionType::JMZ, static_cast<int64_t>(0));
  visit(ctx->statement());
  code.emplace_back(InstructionType::JMP, startIp);
  const int64_t endIp = code.size();
  code[jmzIndex].intOperand = endIp;
  return nullptr;
}

antlrcpp::Any GrammarASTInterpreter::visitFor(GrammarParser::ForContext *ctx) {
  code.emplace_back(InstructionType::ENTER_SCOPE);

  if (ctx->forInit()) {
    visit(ctx->forInit());
  }

  const size_t conditionStart = code.size();

  if (ctx->forCondition()) {
    visit(ctx->forCondition());
  } else {
    code.emplace_back(InstructionType::PUSH_BOOL, true);
  }

  const size_t jmzIndex = code.size();
  code.emplace_back(InstructionType::JMZ, static_cast<int64_t>(0));
  visit(ctx->statement());

  if (ctx->forIncrement()) {
    visit(ctx->forIncrement());
  }

  code.emplace_back(InstructionType::JMP, static_cast<int64_t>(conditionStart));

  const size_t endPos = code.size();
  code[jmzIndex].intOperand = static_cast<int64_t>(endPos);

  code.emplace_back(InstructionType::EXIT_SCOPE);

  return nullptr;
}

antlrcpp::Any GrammarASTInterpreter::visitForInit(GrammarParser::ForInitContext *ctx) {
  if (ctx->vardef()) {
    return visit(ctx->vardef());
  }
  if (ctx->assign()) {
    return visit(ctx->assign());
  }
  return nullptr;
}

antlrcpp::Any GrammarASTInterpreter::visitForIncrement(GrammarParser::ForIncrementContext *ctx) {
  if (ctx->assign()) {
    return visit(ctx->assign());
  }
  return nullptr;
}

antlrcpp::Any GrammarASTInterpreter::visitVardef(GrammarParser::VardefContext *ctx) {
  auto i = ctx->ID()->getText();
  visit(ctx->expr());
  code.emplace_back(InstructionType::STORE_VAR, ctx->ID()->getText());
  return nullptr;
}

antlrcpp::Any GrammarASTInterpreter::visitAssign(GrammarParser::AssignContext *ctx) {
  visit(ctx->expr());
  code.emplace_back(InstructionType::STORE_VAR, ctx->ID()->getText());
  return nullptr;
}

antlrcpp::Any GrammarASTInterpreter::visitPrint(GrammarParser::PrintContext *ctx) {
  if (ctx->expr()) visit(ctx->expr());
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
  auto str = ctx->ID()->getText();
  code.emplace_back(InstructionType::PUSH_VAR, ctx->ID()->getText());
  return nullptr;
}

antlrcpp::Any GrammarASTInterpreter::visitInteger(GrammarParser::IntegerContext *ctx) {
  int64_t val = std::stoll(ctx->INT()->getText());
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
  const size_t count = ctx->expr_list()->expr().size();
  code.emplace_back(InstructionType::NEW_ARRAY, static_cast<int64_t>(count));
  for (int64_t i = 0; i < count; i++) {
    code.emplace_back(InstructionType::DUP_TOP);
    code.emplace_back(InstructionType::PUSH_INT, i);
    visit(ctx->expr_list()->expr(i));
    code.emplace_back(InstructionType::SET_ELEMENT);
  }
  return nullptr;
}

antlrcpp::Any GrammarASTInterpreter::visitIndex(GrammarParser::IndexContext *ctx) {
  code.emplace_back(InstructionType::PUSH_VAR, ctx->ID()->getText());
  visit(ctx->expr());
  code.emplace_back(InstructionType::GET_ELEMENT);
  return nullptr;
}

antlrcpp::Any GrammarASTInterpreter::visitArraySize(GrammarParser::ArraySizeContext *ctx) {
  visit(ctx->expr());
  auto instruction = Instruction {
    InstructionType::NEW_ARRAY,
  };
  instruction.strOperand = "__stack";
  code.emplace_back(instruction);
  return nullptr;
}

antlrcpp::Any GrammarASTInterpreter::visitMethodCall(GrammarParser::MethodCallContext *ctx) {
  std::cout << 1;
  return GrammarBaseVisitor::visitMethodCall(ctx);
}

antlrcpp::Any GrammarASTInterpreter::visitElementAssign(GrammarParser::ElementAssignContext *ctx) {
  code.emplace_back(InstructionType::PUSH_VAR, ctx->ID()->getText());
  visit(ctx->expr(0));
  visit(ctx->expr(1));
  code.emplace_back(InstructionType::SET_ELEMENT);
  return nullptr;
}

antlrcpp::Any GrammarASTInterpreter::visitFunction(GrammarParser::FunctionContext *ctx) {
  const std::string funcName = ctx->ID()->getText();

  const std::size_t startAddr = code.size();
  std::vector<std::string> params;
  if (ctx->formal_args()) {
    for (const auto fa: ctx->formal_args()->formal_arg()) {
      std::string argName = fa->ID()->getText();
      params.emplace_back(argName);
    }
  }

  functionTable[funcName] = FunctionInfo{
    funcName,
    params,
    static_cast<int64_t>(startAddr)
  };

  visitBlock(ctx->block());
  code.emplace_back(InstructionType::RET);
  return nullptr;
}

antlrcpp::Any GrammarASTInterpreter::visitReturn(GrammarParser::ReturnContext *ctx) {
  visit(ctx->expr());
  code.emplace_back(InstructionType::RET);
  return nullptr;
}

antlrcpp::Any GrammarASTInterpreter::visitCallStatement(GrammarParser::CallStatementContext *ctx) {
  return GrammarBaseVisitor::visitCallStatement(ctx);
}


antlrcpp::Any GrammarASTInterpreter::visitCall_expr(GrammarParser::Call_exprContext *ctx) {
  if (ctx->expr_list()) {
    for (const auto e: ctx->expr_list()->expr()) {
      visit(e);
    }
  }

  std::string funcName = ctx->ID()->getText();
  code.emplace_back(InstructionType::CALL, funcName);

  return nullptr;
}
