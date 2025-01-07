#include <gtest/gtest.h>

#include "RunTests.h"

class LanguageTests : public ::testing::TestWithParam<bool> {
};

TEST_P(LanguageTests, FactorialTest) {
  bool isOptimised = GetParam();
  std::string factorialCode = R"(
    {
        var value = 1
        for (var i = 2; i <= 20; i = i + 1) {
            value = value * i
        }

        print(value)
    }
    )";

  std::string expectedOutput = "2432902008176640000\n"; // 20!
  std::string actualOutput = runCode(factorialCode, isOptimised);
  EXPECT_EQ(actualOutput, expectedOutput);
}

TEST_P(LanguageTests, MergeSortTest) {
  bool isOptimised = GetParam();
  std::string mergeSortCode = R"(
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
        mergeSort(vec, 0, n - 1)
        for (var i = 0; i < __size(vec); i = i + 1) {
            print(vec[i])
        }
    }
    )";

  std::string expectedOutput = "-5\n1\n2.4\n2.5\n3\n";
  std::string actualOutput = runCode(mergeSortCode, isOptimised);
  EXPECT_EQ(actualOutput, expectedOutput);
}

TEST_P(LanguageTests, SieveOfEratosthenesTest) {
  bool isOptimised = GetParam();
  std::string sieveCode = R"(
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
            sieveOfEratosthenes(10)
        }
    )";

  std::string expectedOutput = "2\n3\n5\n7\n";
  std::string actualOutput = runCode(sieveCode, isOptimised);
  EXPECT_EQ(actualOutput, expectedOutput);
}

TEST_P(LanguageTests, OperationPriority) {
  bool isOptimised = GetParam();
  std::string experimentCode = R"(
        {
          print(2 + 2 * 2)
        }
    )";

  std::string expectedOutput = "6\n";
  std::string actualOutput = runCode(experimentCode, isOptimised);
  EXPECT_EQ(actualOutput, expectedOutput);
}

TEST_P(LanguageTests, GarbageCollectorWithGlobalValues) {
  bool isOptimised = GetParam();
  std::string experimentCode = R"(
        var a = [1, 2, 3]
    )";

  antlr4::ANTLRInputStream inputStream(experimentCode);
  GrammarLexer lexer(&inputStream);
  antlr4::CommonTokenStream tokens(&lexer);
  GrammarParser parser(&tokens);

  GrammarParser::ScriptContext *tree = parser.script();

  GrammarASTInterpreter visitor;
  visitor.visit(tree);

  GarbageCollector gc;

  VirtualMachine vm(visitor.code, visitor.functionTable, &gc, visitor.startPos);
  vm.optimize(isOptimised);
  vm.run();

  EXPECT_EQ(vm.getHeapSize(), 0);
}

TEST_P(LanguageTests, GarbageCollector) {
  bool isOptimised = GetParam();
  std::string experimentCode = R"(
    {
        var a = [1, 2, 3]
    }
    )";

  antlr4::ANTLRInputStream inputStream(experimentCode);
  GrammarLexer lexer(&inputStream);
  antlr4::CommonTokenStream tokens(&lexer);
  GrammarParser parser(&tokens);

  GrammarParser::ScriptContext *tree = parser.script();

  GrammarASTInterpreter visitor;
  visitor.visit(tree);

  GarbageCollector gc;

  VirtualMachine vm(visitor.code, visitor.functionTable, &gc, visitor.startPos);
  vm.optimize(isOptimised);
  vm.run();

  EXPECT_EQ(vm.getHeapSize(), 0);
}

TEST_P(LanguageTests, SumWithDifferenceType) {
  bool isOptimised = GetParam();
  std::string experimentCode = R"(
    {
        var a = 1
        var b = 3.2
        print(a + b)
    }
    )";

  std::string expectedOutput = "4.2\n";
  std::string actualOutput = runCode(experimentCode, isOptimised);
  EXPECT_EQ(actualOutput, expectedOutput);
}

INSTANTIATE_TEST_SUITE_P(
  FlagOnOff,
  LanguageTests,
  ::testing::Bool()
);

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
