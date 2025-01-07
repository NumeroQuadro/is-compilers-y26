#pragma once

#include <string>
#include <sstream>
#include <iostream>
#include "antlr4-runtime.h"
#include "GarbageCollector.h"
#include "GrammarLexer.h"
#include "GrammarParser.h"
#include "GrammarASTInterpreter.h"
#include "VirtualMachine.h"

std::string runCode(const std::string& codeStr, bool withOptimisation = false) {
  std::stringstream buffer;
  std::streambuf* oldCoutBuffer = std::cout.rdbuf(buffer.rdbuf());

  antlr4::ANTLRInputStream inputStream(codeStr);
  GrammarLexer lexer(&inputStream);
  antlr4::CommonTokenStream tokens(&lexer);
  GrammarParser parser(&tokens);

  GrammarParser::ScriptContext *tree = parser.script();

  GrammarASTInterpreter visitor;
  visitor.visit(tree);

  GarbageCollector gc;

  VirtualMachine vm(visitor.code, visitor.functionTable, &gc, visitor.startPos);
  vm.optimize(withOptimisation);
  vm.run();

  std::cout.rdbuf(oldCoutBuffer);

  return buffer.str();
}