#include "GrammarASTInterpreter.h"

Value &GrammarASTInterpreter::getVariable(const std::string &variableName) {
  auto it = globalScope.find(variableName);
  if (it == globalScope.end()) {
    throw std::runtime_error("Variable not found");
  }

  return it->second;
}

bool GrammarASTInterpreter::isTruthy(const Value &val) {
  switch (val.type) {
    case Value::Type::BOOL:   return val.asBool;
    case Value::Type::INT:    return val.asInt != 0;
    case Value::Type::FLOAT:  return val.asFloat != 0.0f;
    case Value::Type::STRING: return !val.asString.empty();
    case Value::Type::VECTOR: return !val.asVector.empty();
  }
  return false;
}

Value GrammarASTInterpreter::callFunction(const std::string &functionName, const std::vector<Value> &args) {
  auto it = functionMap.find(functionName);
  if (it == functionMap.end()) {
    throw std::runtime_error("Function not found");
  }

  auto functionCtx = it->second;

  std::map<std::string, Value> localScope;

  GrammarParser::Formal_argsContext *formal_args_context = functionCtx->formal_args();
  size_t paramCount = 0;
  if (formal_args_context) {
    paramCount = formal_args_context->formal_arg().size();
    if (paramCount != args.size()) {
      throw std::runtime_error("Wrong number of arguments");
    }

    for (size_t i = 0; i < paramCount; i++) {
      auto argName = formal_args_context->formal_arg(i)->ID()->getText();
      localScope[argName] = args[i];
    }
  } else {
    if (!args.empty()) {
      throw std::runtime_error("Wrong number of arguments");
    }
  }

  std::map<std::string, Value> oldScope = globalScope;
  for (const auto &[fst, snd]: globalScope) {
    globalScope[fst] = snd;
  }

  Value returnValue;
  auto blockContext = functionCtx->block();
  for (auto item: blockContext->children) {
    Value result;
    try {
      result = evaluate(item);
    } catch (const std::string &signalReturn) {
      // TODO
    }

    if (result.type == Value::Type::STRING && result.asString == "__return_signal__") {
      break;
    }

    if (result.type == Value::Type::BOOL && result.asBool == true && result.asString == "HAS_RETURN_VALUE") {
    }
  }

  globalScope = oldScope;
  return returnValue;
}

Value GrammarASTInterpreter::evaluate(antlr4::tree::ParseTree* node) {
  if (!node) {
    return Value::makeInt(0);
  }

  antlrcpp::Any value = node->accept(this);

  if (const auto result = std::any_cast<Value>(&value)) {
    return *result;
  }

  return Value::makeInt(0);
}

antlrcpp::Any GrammarASTInterpreter::visitScript(GrammarParser::ScriptContext *context) {
  for (const auto& vd : context->vardef()) {
    visitVardef(vd);
  }

  for (const auto& fn : context->function()) {
    visitFunction(fn);
  }

  for (const auto& st : context->statement()) {
    evaluate(st);
  }

  return nullptr;
}

antlrcpp::Any GrammarASTInterpreter::visitFunction(GrammarParser::FunctionContext *context) {
  return GrammarBaseVisitor::visitFunction(context);
}

antlrcpp::Any GrammarASTInterpreter::visitBlockStatement(GrammarParser::BlockStatementContext *context) {
  return GrammarBaseVisitor::visitBlockStatement(context);
}

antlrcpp::Any GrammarASTInterpreter::visitBlock(GrammarParser::BlockContext *context) {
  return GrammarBaseVisitor::visitBlock(context);
}

antlrcpp::Any GrammarASTInterpreter::visitReturn(GrammarParser::ReturnContext *context) {
  return GrammarBaseVisitor::visitReturn(context);
}

antlrcpp::Any GrammarASTInterpreter::visitCallStatement(GrammarParser::CallStatementContext *context) {
  return GrammarBaseVisitor::visitCallStatement(context);
}

antlrcpp::Any GrammarASTInterpreter::visitCall(GrammarParser::CallContext *context) {
  return GrammarBaseVisitor::visitCall(context);
}

antlrcpp::Any GrammarASTInterpreter::visitCall_expr(GrammarParser::Call_exprContext *context) {
  return GrammarBaseVisitor::visitCall_expr(context);
}

antlrcpp::Any GrammarASTInterpreter::visitVardef(GrammarParser::VardefContext *context) {
  const std::string variableName = context->ID()->getText();
  const Value value = evaluate(context->expr());
  globalScope[variableName] = value;
  return nullptr;
}

antlrcpp::Any GrammarASTInterpreter::visitIf(GrammarParser::IfContext *context) {
  Value cond = evaluate(context->expr());
  if (isTruthy(cond)) {
    evaluate(context->statement(0));
  } else if (context->statement().size() > 1) {
    evaluate(context->statement(1));
  }

  return nullptr;
}

antlrcpp::Any GrammarASTInterpreter::visitWhile(GrammarParser::WhileContext *context) {
  return GrammarBaseVisitor::visitWhile(context);
}

antlrcpp::Any GrammarASTInterpreter::visitAssign(GrammarParser::AssignContext *context) {
  std::string variableName = context->ID()->getText();
  Value value = evaluate(context->expr());
  globalScope[variableName] = value;
  return nullptr;
}

antlrcpp::Any GrammarASTInterpreter::visitElementAssign(GrammarParser::ElementAssignContext *context) {
  return GrammarBaseVisitor::visitElementAssign(context);
}

antlrcpp::Any GrammarASTInterpreter::visitPrint(GrammarParser::PrintContext *context) {
  if (context->expr()) {
    Value value = evaluate(context->expr());
    std::cout << value.toString() << std::endl;
  } else {
    std::cout << std::endl;
  }

  return nullptr;
}

antlrcpp::Any GrammarASTInterpreter::visitOp(GrammarParser::OpContext *context) {
  Value left = evaluate(context->expr(0));
  Value right = evaluate(context->expr(1));
  std::string op = context->operator_()->getText();

  if (op == "+") {
    return left + right;
  }
  if (op == "*") {
    return left * right;
  }
  if (op == ">") {
    return left > right;
  }
}

antlrcpp::Any GrammarASTInterpreter::visitNegate(GrammarParser::NegateContext *context) {
  return GrammarBaseVisitor::visitNegate(context);
}

antlrcpp::Any GrammarASTInterpreter::visitNot(GrammarParser::NotContext *context) {
  return GrammarBaseVisitor::visitNot(context);
}

antlrcpp::Any GrammarASTInterpreter::visitParens(GrammarParser::ParensContext *context) {
  return GrammarBaseVisitor::visitParens(context);
}

antlrcpp::Any GrammarASTInterpreter::visitAtom(GrammarParser::AtomContext *context) {
  return GrammarBaseVisitor::visitAtom(context);
}

antlrcpp::Any GrammarASTInterpreter::visitIdentifier(GrammarParser::IdentifierContext *ctx) {
  std::string varName = ctx->getText();
  return getVariable(varName);
}

antlrcpp::Any GrammarASTInterpreter::visitInteger(GrammarParser::IntegerContext *ctx) {
  int val = std::stoi(ctx->getText());
  return Value::makeInt(val);
}

antlrcpp::Any GrammarASTInterpreter::visitFloat(GrammarParser::FloatContext *ctx) {
  float val = std::stof(ctx->getText());
  return Value::makeFloat(val);
}

antlrcpp::Any GrammarASTInterpreter::visitString(GrammarParser::StringContext *ctx) {
  std::string text = ctx->getText();
  if (text.size() >= 2 && text[0] == '"' && text[text.size() - 1] == '"') {
    text = text.substr(1, text.size() - 2);
  }
  return Value::makeString(text);
}

antlrcpp::Any GrammarASTInterpreter::visitVector(GrammarParser::VectorContext *ctx) {
  std::vector<Value> elements;
  auto exprs = ctx->expr_list()->expr();
  for (auto e : exprs) {
    elements.push_back(evaluate(e));
  }
  return Value::makeVector(elements);
}

antlrcpp::Any GrammarASTInterpreter::visitTrueLiteral(GrammarParser::TrueLiteralContext *ctx) {
  return Value::makeBool(true);
}

antlrcpp::Any GrammarASTInterpreter::visitFalseLiteral(GrammarParser::FalseLiteralContext *ctx) {
  return Value::makeBool(false);
}