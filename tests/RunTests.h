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

size_t runCodeAndGetOperationsCount(const std::string& codeStr, bool withOptimisation = false) {

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

    return vm.getRunnedOperationsCount();
}

size_t getTimeScoreOfLanguage(const std::string &code, bool optimize) {
    using namespace std::chrono;
    auto start = high_resolution_clock::now();

    antlr4::ANTLRInputStream inputStream(code);
    GrammarLexer lexer(&inputStream);
    antlr4::CommonTokenStream tokens(&lexer);
    GrammarParser parser(&tokens);
    GrammarParser::ScriptContext *tree = parser.script();

    GrammarASTInterpreter visitor;
    visitor.visit(tree);
    GarbageCollector gc;

    VirtualMachine vm(visitor.code, visitor.functionTable, &gc, visitor.startPos);
    vm.optimize(optimize);
    vm.run();

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start);

    return duration.count();
}

std::string getPrimes(int n) {
    std::vector<char> prime(n + 1, true);
    prime[0] = prime[1] = false;
    for (int i = 2; i <= n; ++i)
        if (prime[i])
            if (i * 1ll * i <= n)
                for (int j = i * i; j <= n; j += i)
                    prime[j] = false;

    std::string str;
    for (int i = 0; i < n; i++) {
        if (prime[i]) {
            str += std::to_string(i) + "\n";
        }
    }

    return str;
}