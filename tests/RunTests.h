#pragma once

#include <string>
#include <sstream>
#include <iostream>
#include "antlr4-runtime.h"
#include "GrammarLexer.h"
#include "GrammarParser.h"
#include "GrammarASTInterpreter.h"
#include "VirtualMachine.h"

std::string runCode(const std::string& codeStr) {
  std::stringstream buffer;
  std::streambuf* oldCoutBuffer = std::cout.rdbuf(buffer.rdbuf());

  antlr4::ANTLRInputStream inputStream(codeStr);
  GrammarLexer lexer(&inputStream);
  antlr4::CommonTokenStream tokens(&lexer);
  GrammarParser parser(&tokens);

  GrammarParser::ScriptContext *tree = parser.script();

  GrammarASTInterpreter visitor;
  visitor.visit(tree);

  VirtualMachine vm(visitor.code, visitor.functionTable, visitor.startPos);
  vm.optimize(true);
  vm.run();

  std::cout.rdbuf(oldCoutBuffer);

  return buffer.str();
}