#include "GrammarLexer.h"
#include "GrammarParser.h"
#include "CalculatorInterpreter.h"

#include <iostream>
#include <antlr4-runtime.h>

class ExceptionErrorListener : public antlr4::BaseErrorListener {
public:
  virtual void syntaxError(
    antlr4::Recognizer *recognizer, antlr4::Token *offendingSymbol,
    size_t line, size_t charPositionInLine, const std::string &msg,
    std::exception_ptr e) override {
    e = nullptr;
    std::ostringstream oss;
    oss << "line: " << line << ":" << charPositionInLine << " " << msg;
    error_msg = oss.str();
    throw antlr4::ParseCancellationException(error_msg);
  }

private:
  std::string error_msg;
};

int main() {
  std::string example = R"(
    if 5 > 3 {
      2 + 3 + 1
    } else {
      10 - 5
    }
  )";

  antlr4::ANTLRInputStream input(example);

  GrammarLexer lexer(&input);
  lexer.removeErrorListeners();
  lexer.addErrorListener(new ExceptionErrorListener());

  antlr4::CommonTokenStream tokens(&lexer);

  GrammarParser parser(&tokens);
  parser.removeErrorListeners();
  parser.addErrorListener(new ExceptionErrorListener());

  antlr4::tree::ParseTree *tree = nullptr;

  try {
    tree = parser.input();
  } catch (antlr4::ParseCancellationException &e) {
    std::cout << "Syntax Error: " << e.what() << std::endl;
    return 1;
  }

  CalculatorInterpreter interpreter;

  // Используем Visitor для обхода дерева
  interpreter.visit(tree);

  // Вывод результата
  std::cout << "Result: " << interpreter.get_result() << std::endl;

  return 0;
}
