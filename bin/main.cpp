// #include "GrammarLexer.h"
// #include "GrammarParser.h"
// #include "GrammarASTInterpreter.h"
//
// #include <iostream>
// #include <antlr4-runtime.h>
//
// class ExceptionErrorListener : public antlr4::BaseErrorListener {
// public:
//   virtual void syntaxError(
//     antlr4::Recognizer *recognizer, antlr4::Token *offendingSymbol,
//     size_t line, size_t charPositionInLine, const std::string &msg,
//     std::exception_ptr e) override {
//     e = nullptr;
//     std::ostringstream oss;
//     oss << "line: " << line << ":" << charPositionInLine << " " << msg;
//     error_msg = oss.str();
//     throw antlr4::ParseCancellationException(error_msg);
//   }
//
// private:
//   std::string error_msg;
// };
//
// int main() {
//   std::string example = R"(
//     var b = 3;
//     if (b > 2) {
//       b = 52
//     } else {
//       b = 12
//     }
//
//     print(b);
//   )";
//
//   antlr4::ANTLRInputStream input(example);
//
//   GrammarLexer lexer(&input);
//   lexer.removeErrorListeners();
//   lexer.addErrorListener(new ExceptionErrorListener());
//
//   antlr4::CommonTokenStream tokens(&lexer);
//
//   GrammarParser parser(&tokens);
//   parser.removeErrorListeners();
//   parser.addErrorListener(new ExceptionErrorListener());
//
//   antlr4::tree::ParseTree *tree = nullptr;
//
//   try {
//     tree = parser.script();
//   } catch (antlr4::ParseCancellationException &e) {
//     std::cout << "Syntax Error: " << e.what() << std::endl;
//     return 1;
//   }
//
//   GrammarASTInterpreter interpreter;
//
//   // Используем Visitor для обхода дерева
//   interpreter.visit(tree);
//
//   return 0;
// }


#include <iostream>
#include "antlr4-runtime.h"
#include "GrammarLexer.h"
#include "GrammarParser.h"
#include "GrammarASTInterpreter.h"
#include "VirtualMachine.h"

int main() {
  std::string factorial = R"(
  {
    var value = 1
    for (var i = 2; i <= 20; i = i + 1) {
      value = value * i
    }

    print(value)
  }
  )";

  std::string arraySorting = R"(
func partition(vec: [int], low: int, high: int) {
  var pivot = vec[high]
  var = low - 1

  for (int j = low; j <= high - 1; j = j + 1) {
    if (vec[j] <= pivot) {
      i = i + 1
      var tmp = vec[i]
      vec[i] = vec[j]
      vec[j] = tmp
    }
  }

  var tmp = vec[i + 1]
  vec[i + 1] = vec[j]
  vec[j] = tmp

  i = i + 1
  return i
}

func quickSort(vec: [int], low: int, high: int) {
  if (low < high) {
    int pi = partition(vec, low, high)

    quickSort(vec, low, pi - 1)
    quickSort(vec, pi + 1, high)
  }
}

{
  vec = [4, 3, 2, 1]
  n = 4

  quickSort(vec, 0, n - 1)
  for (var i = 0; i < n; i++) {
    print(i)
  }
}
)";

  std::string sieveOfEratosthenes = R"(

)";

  std::string experiment = R"(
{
    var arr = [..4]
    __pushBack(arr, 3)

    print(__size(arr))
    for (var i = 0; i < 5; i = i + 1) {
      print(arr[i])
    }

    __popBack(arr)
    print(__size(arr))
}
  )";

  antlr4::ANTLRInputStream inputStream(experiment);
  GrammarLexer lexer(&inputStream);
  antlr4::CommonTokenStream tokens(&lexer);
  GrammarParser parser(&tokens);

  GrammarParser::ScriptContext* tree = parser.script();

  GrammarASTInterpreter visitor;
  visitor.visit(tree);

  VirtualMachine vm(visitor.code, visitor.functionTable, visitor.startPos);
  vm.run();

  return 0;
}
