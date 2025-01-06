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
    std::string factorial = R"(
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
        var n = 5
        var vec = [1, 3, 2.5, 2.4, -5]

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
                print(i)
            }
        }
    }

    {
        sieveOfEratosthenes(10000)
    }
    )";

    std::string experiment = R"(
    func test(in: num) {
        var r = in + 3
        var e = 2 + 4 - 1
      var d = !true or (!true or false) and true or (5 > 3) and (7 < in) or false
      var c = a + 2
        var m = 3 + 2 + in + 8
var w = 8 * 3 + in
print(w)
      print(d)
    if (true or false) {
        print(5)
        print(r)
    }
print(m)
        print(e)
print(8 + 1)
    }
    {
      test(34)
      print("finish")
    }
  )";
    std::string experiment2 = R"(
    func red() {
        print (9 * 7 + 1)
        print("gol")
    }
    func test(in: num) {
        while (false or false and true) {
            print(6 * 8 + 9)
            return true
        }
if (in > 0) {
print((8 * 3 + 1) + in + (5 * 3))
}
        for (var i = 8 + 7 * 8 - 7; i < 100 - 1 + (-3); i = i + 1) {
            print(i)
        }

        var prime = [..10 + 4]
        for (var i = 0; i < __size(prime); i = i + 1) {
            prime[i] = 1 + 3 + 4 + in + 5
        }
print(in)
for (var i = 0; i < __size(prime); i = i + 1) {
            print(prime[i])
        }
    }
    {
      test(34)
        red()
      print("finish")
    }
  )";

    std::string experiment3 = R"(
    func red() {
        print (9 * 7 + 1)
        print("gol")
    }
    func test(in: num, out: num) {
        return 5 + 3 * 2
    }
    {
      print(test(34 + 2 * 3, 3 + 2))
        red()
      print("finish")
    }
  )";

    std::string factorial2 = R"(
    func factorial(a: num) {
        if (a == 1 or a == 0) {
            return 1
        }
        return a * factorial(a - 1)
    }
    {
      print(factorial(19))
    }
  )";
    std::string experiment4 = R"(
    func red() {
        print (9 * 7 + 1)
        print("gol")
    }
    func test(z: num, in: num, out: num) {
        if (z < 12 and in == 10 and out == 11) {
            return 4
        }
        print(in)
        test(z - 1, in + 1, 2 * 5 + 1)
        print(z)
    }
    {
        test(0, 2 * 3, 3 + 2)
        red()
        print("finish")
    }
    )";


    antlr4::ANTLRInputStream inputStream(factorial2);
    GrammarLexer lexer(&inputStream);
    antlr4::CommonTokenStream tokens(&lexer);
    GrammarParser parser(&tokens);

    GrammarParser::ScriptContext *tree = parser.script();

    GrammarASTInterpreter visitor;
    visitor.visit(tree);
    // visitor.toFile("test.txt");
    //
    // VirtualMachine vm;
    // vm.optimize(true);
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

    VirtualMachine vm(visitor.code, visitor.functionTable, visitor.startPos);
    vm.optimize(true);
    auto instructions = vm.getInstructions();
//    for (int i = 0; i < instructions.size(); i++) {
//        std::cout << i << " " << instructions[i].toStr() << '\n';
//    }
    vm.run();
 //   std::cout << "\nOptimized:\n";
   instructions = vm.getInstructions();
//  for (int i = 0; i < instructions.size(); i++) {
//      std::cout << i << " " << instructions[i].toStr() << '\n';
//  }

    return 0;
}

using namespace std;

int partition(vector<int> &vec, int low, int high) {
    // Selecting last element as the pivot
    int pivot = vec[high];

    // Index of elemment just before the last element
    // It is used for swapping
    int i = (low - 1);

    for (int j = low; j <= high - 1; j++) {
        // If current element is smaller than or
        // equal to pivot
        if (vec[j] <= pivot) {
            i++;
            swap(vec[i], vec[j]);
        }
    }

    // Put pivot to its position
    swap(vec[i + 1], vec[high]);

    // Return the point of partition
    return (i + 1);
}

void quickSort(vector<int> &vec, int low, int high) {
    // Base case: This part will be executed till the starting
    // index low is lesser than the ending index high
    cout << low;
    cout << high;
    if (low < high) {
        // pi is Partitioning Index, arr[p] is now at
        // right place
        int pi = partition(vec, low, high);
        cout << pi;
        // Separately sort elements before and after the
        // Partition Index pi
        quickSort(vec, low, pi - 1);
        quickSort(vec, pi + 1, high);
    }
}

int main2() {
    vector<int> vec = {4, 3, 2, 1};
    int n = vec.size();

    // Calling quicksort for the vector vec
    quickSort(vec, 0, n - 1);

    for (auto i: vec) {
        cout << i << " ";
    }
    return 0;
}
