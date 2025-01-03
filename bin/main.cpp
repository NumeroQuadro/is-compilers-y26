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


#include <cmath>
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

  std::string mergeSort = R"(
func merge (arr: [], left: int, mid: int, right: int) {
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

  while (i < n1 && j < n2) {
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

func mergeSort(arr: [], left: int, right: int) {
  if (left >= right) {
    return 0
  }

  var tmp = right - left
  tmp = tmp / 2
  var mid = left + tmp
  mergeSort(arr, left, mid)
  mergeSort(arr, mid + 1, right)
  // merge(arr, left, mid, right)
}

{
  var n = 10000
  var vec = [..n]
  for (var i = 0; i < n; i = i + 1) {
    vec[i] = n - i
  }

  mergeSort(vec, 0, n - 1)
  for (var i = 0; i < n; i = i + 1) {
    print(vec[i])
  }
}
)";

  std::string sieveOfEratosthenes = R"(

)";

  std::string experiment = R"(
func test(left: int, right: int) {
var res = left + right
var a = (5 == 5)
if (a == true) {
    print(a)
}
var c = 3
var z = (3 + c + 4) * 3
var b = 2
return res
}
{
    var value = test(62, 54)
    var value = test(1, 2)
    print(value)
  }
  )";

  antlr4::ANTLRInputStream inputStream(experiment);
  GrammarLexer lexer(&inputStream);
  antlr4::CommonTokenStream tokens(&lexer);
  GrammarParser parser(&tokens);

  GrammarParser::ScriptContext* tree = parser.script();

  GrammarASTInterpreter visitor;
  visitor.visit(tree);
  visitor.toFile("test.txt");

  VirtualMachine vm;
  vm.optimize(true);
  vm.fromFile("test.txt");
  auto code = vm.getInstructions();
  for (int i = 0; i < code.size(); i++) {
      if (code[i] != visitor.code[i]) {
          std::cout << "Error in instruction " + std::to_string(i) + "\n";
          std::cout << code[i].toStr() << "\n";
          std::cout << visitor.code[i].toStr() << "\n";
      }
  }
  vm.run();

//    VirtualMachine vm(visitor.code, visitor.functionTable, visitor.startPos);
//    vm.run();

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

  for (auto i : vec) {
    cout << i << " ";
  }
  return 0;
}