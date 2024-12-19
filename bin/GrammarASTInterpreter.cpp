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

