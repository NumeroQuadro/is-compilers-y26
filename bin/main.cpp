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

void getTimeScoreOfLanguage(const std::string &code) {
  using namespace std::chrono;
  auto start = high_resolution_clock::now();

  antlr4::ANTLRInputStream inputStream(code);
  GrammarLexer lexer(&inputStream);
  antlr4::CommonTokenStream tokens(&lexer);
  GrammarParser parser(&tokens);
  GrammarParser::ScriptContext *tree = parser.script();

  GrammarASTInterpreter visitor;
  visitor.visit(tree);

  VirtualMachine vm(visitor.code, visitor.functionTable, visitor.startPos);
  vm.run();

  auto end = high_resolution_clock::now();
  auto duration = duration_cast<milliseconds>(end - start);

  std::cout << "Execution time: " << duration.count() << " ms" << std::endl;
}


int main() {
  const std::string factorial = R"(
    {
        var value = 1
        for (var i = 2; i <= 20; i = i + 1) {
            value = value * i
        }
        print(value)
    }
    )";

  const std::string mergeSort = R"(
    func merge (arr: [], left: num, mid: num, right: num) {
        var n1 = mid - left + 1
        var n2 = right - mid

        var l = [..n1]
        var r = [..n2]

        for (var i = 0; i < n1; i = i + 1) {
            l[i] = arr[left + i]
        }

        for (var i = 0; i < n2; i = i + 1) {
            r[i] = arr[mid + 1 + i]
        }
        var i = 0
        var j = 0
        var k = left

        while (i < n1 and j < n2) {
            if (l[i] <= r[j]) {
                arr[k] = l[i]
                i = i + 1
            } else {
                arr[k] = r[j]
                j = j + 1
            }
            k = k + 1
        }

        while (i < n1) {
            arr[k] = l[i]
            i = i + 1
            k = k + 1
        }

        while (j < n2) {
            arr[k] = r[j]
            j = j + 1
            k = k + 1
        }
    }

    func mergeSort(arr: [], left: num, right: num) {
        if (left >= right) {
            return 0
        }

        var tmp = right - left
        tmp = tmp / 2
        var mid = left + tmp
        mergeSort(arr, left, mid)
        mergeSort(arr, mid + 1, right)
        merge(arr, left, mid, right)
    }

    {
        var n = 4
        var vec = [1, 3, 2, -5]
//        for (var i = 0; i < n; i = i + 1) {
//            vec[i] = n
//        }
        mergeSort(vec, 0, n - 1)
        for (var i = 0; i < __size(vec); i = i + 1) {
            print(vec[i])
        }
    }
    )";

  const std::string sieveOfEratosthenes = R"(
    func sieveOfEratosthenes(n: num) {
        var prime = [..n + 1]
        for (var i = 0; i < __size(prime); i = i + 1) {
            prime[i] = 1
        }

        for (var p = 2; p <= n; p = p + 1) {
            if (prime[p] == true) {
                for (var i = p * p; i <= n; i = i + p) {
                    prime[i] = false
                }
            }
        }

        for (var i = 2; i <= n; i = i + 1) {
            if (prime[i] == true) {
                // print(i)
            }
        }
    }

    {
        sieveOfEratosthenes(10000)
    }
    )";

  std::string experiment = R"(
    {
        var a = 1
        var b = a + a - 3
        print(b)
    }
    )";

    antlr4::ANTLRInputStream inputStream(mergeSort);
    GrammarLexer lexer(&inputStream);
    antlr4::CommonTokenStream tokens(&lexer);
    GrammarParser parser(&tokens);
    GrammarParser::ScriptContext *tree = parser.script();

    GrammarASTInterpreter visitor;
    visitor.visit(tree);

    VirtualMachine vm(visitor.code, visitor.functionTable, visitor.startPos);
    vm.run();

  //
  // GrammarASTInterpreter visitor;
  // visitor.visit(tree);
  // visitor.toFile("test.txt");
  //
  // VirtualMachine vm;
  // vm.fromFile("test.txt");
  // auto code = vm.getInstructions();
  // for (int i = 0; i < code.size(); i++) {
  //     if (code[i] != visitor.code[i]) {
  //         std::cout << "Error in instruction " + std::to_string(i) + "\n";
  //         std::cout << code[i].toStr() << "\n";
  //         std::cout << visitor.code[i].toStr() << "\n";
  //     }
  // }
  // vm.run();


  return 0;
}
