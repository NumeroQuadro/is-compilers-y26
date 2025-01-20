#include <gtest/gtest.h>

#include "RunTests.h"

class LanguageTests : public ::testing::TestWithParam<bool> {
};

TEST(Task4, wihoutO2) {
    std::string factorialCode = R"(
    func itemCheck(node: []) {
        if (node[0] == -1) {
            return node[2]
        }
        return node[2] + itemCheck(node[0]) - itemCheck(node[1])
    }
    func bottomUpTree(item: num, depth: num) {
        if (depth > 0) {
            return [bottomUpTree(2 * item - 1, depth - 1), bottomUpTree(2 * item, depth - 1), item]
        }
        return [-1, -1, item]
    }
    {
        var ret = 0
        for ( var n = 4; n <= 7; n = n + 1 ) {
            var minDepth = 4
            var maxDepth = 0
            if (minDepth + 2 > n) {
                maxDepth = minDepth + 2
            } else {
                maxDepth = n
            }
            var stretchDepth = maxDepth + 1

            var check = itemCheck(bottomUpTree(0, stretchDepth))
            var longLivedTree = bottomUpTree(0,maxDepth)
            for (var depth = minDepth; depth <= maxDepth; depth = depth + 2){
                var iterations = 1
                for (var j = 0; j < maxDepth - depth + minDepth; j = j + 1) {
                    iterations = iterations * 2
                }

                check = 0
                for (var i = 1; i <= iterations; i = i + 1){
                    check = check + itemCheck(bottomUpTree(i, depth))
                    check = check + itemCheck(bottomUpTree(-i, depth))
                }
            }

            ret = ret +  itemCheck(longLivedTree)
        }

        var expected = -4
        if (ret != expected) {
            print("Error!")
        } else {
            print(ret)
        }
    }
    )";

    std::string expectedOutput = "-4\n";
    std::string actualOutput = runCode(factorialCode, false);
    EXPECT_EQ(actualOutput, expectedOutput);
}

//TEST(Task4, withO2) {
//    std::string factorialCode = R"(
//    func itemCheck(node: []) {
//        if (node[0] == -1) {
//            return node[2]
//        }
//        return node[2] + itemCheck(node[0]) - itemCheck(node[1])
//    }
//    func bottomUpTree(item: num, depth: num) {
//        if (depth > 0) {
//            return [bottomUpTree(2 * item - 1, depth - 1), bottomUpTree(2 * item, depth - 1), item]
//        }
//        return [-1, -1, item]
//    }
//    {
//        var ret = 0
//        for ( var n = 4; n <= 7; n = n + 1 ) {
//            var minDepth = 4
//            var maxDepth = 0
//            if (minDepth + 2 > n) {
//                maxDepth = minDepth + 2
//            } else {
//                maxDepth = n
//            }
//            var stretchDepth = maxDepth + 1
//
//            var check = itemCheck(bottomUpTree(0, stretchDepth))
//            var longLivedTree = bottomUpTree(0,maxDepth)
//            for (var depth = minDepth; depth <= maxDepth; depth = depth + 2){
//                var iterations = 1
//                for (var j = 0; j < maxDepth - depth + minDepth; j = j + 1) {
//                    iterations = iterations * 2
//                }
//
//                check = 0
//                for (var i = 1; i <= iterations; i = i + 1){
//                    check = check + itemCheck(bottomUpTree(i, depth))
//                    check = check + itemCheck(bottomUpTree(-i, depth))
//                }
//            }
//
//            ret = ret +  itemCheck(longLivedTree)
//        }
//
//        var expected = -4
//        if (ret != expected) {
//            print("Error!")
//        } else {
//            print(ret)
//        }
//    }
//    )";
//
//    std::string expectedOutput = "-4\n";
//    std::string actualOutput = runCode(factorialCode, true);
//    EXPECT_EQ(actualOutput, expectedOutput);
//}


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

TEST_P(LanguageTests, Factorial2Test) {
    bool isOptimised = GetParam();
    std::string factorialCode = R"(
    func factorial(a: num) {
        if (a == 1 or a == 0) {
            return 1
        }
        return a * factorial(a - 1)
    }
    {
      print(factorial(20))
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

TEST_P(LanguageTests, MergeSortTest_BigArray) {
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
        var n = 10001
        var vec = [..n]
        for (var i = n - 1; i >= 0; i = i - 1) {
            vec[i] = i % 100
        }
        mergeSort(vec, 0, n - 1)

        var flag = false
        for (var i = 1; i < __size(vec); i = i + 1) {
            if (vec[i] < vec[i - 1]) {
                flag = true
            }
        }
        print(flag)
    }
    )";

    std::string expectedOutput = "false\n";
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

TEST_P(LanguageTests, SieveOfEratosthenesTest_Big) {
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
            sieveOfEratosthenes(100000)
        }
    )";

    std::string expectedOutput = getPrimes(100000);
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

TEST(JITTests, DeadCodeEllumination_HasNotDeadCode_ShouldHaveSameOperationsCount) {
    std::string experimentCode = R"(
    func test() {
        var a = 1
        var b = 3.2
        print(a + b)
    }
    {
        test()
    }
    )";

    size_t operationWithoutOptimizations = runCodeAndGetOperationsCount(experimentCode, false);
    size_t operationWithOptimizations = runCodeAndGetOperationsCount(experimentCode, true);
    EXPECT_EQ(operationWithOptimizations, operationWithoutOptimizations);
}

TEST(JITTests, DeadCodeEllumination_HasDeadCode_ShouldDoLessOperations) {
    std::string experimentCode = R"(
    func test() {
        var a = 1
        var b = 3.2
    }
    {
        test()
    }
    )";

    size_t operationWithoutOptimizations = runCodeAndGetOperationsCount(experimentCode, false);
    size_t operationWithOptimizations = runCodeAndGetOperationsCount(experimentCode, true);
    EXPECT_LT(operationWithOptimizations, operationWithoutOptimizations);
}

TEST(JITTests, DeadCodeEllumination_Array_ShouldDoLessOperations) {
    std::string experimentCode = R"(
    func test() {
        var a = [..10]
    }
    {
        test()
    }
    )";

    size_t operationWithoutOptimizations = runCodeAndGetOperationsCount(experimentCode, false);
    size_t operationWithOptimizations = runCodeAndGetOperationsCount(experimentCode, true);
    EXPECT_LT(operationWithOptimizations, operationWithoutOptimizations);
}

TEST(JITTests, DeadCodeEllumination_Array2_ShouldDoLessOperations) {
    std::string experimentCode = R"(
    func test() {
        var a = [1, 4, -1, 2]
    }
    {
        test()
    }
    )";

    size_t operationWithoutOptimizations = runCodeAndGetOperationsCount(experimentCode, false);
    size_t operationWithOptimizations = runCodeAndGetOperationsCount(experimentCode, true);
    EXPECT_LT(operationWithOptimizations, operationWithoutOptimizations);
}

TEST(JITTests, DeadCodeEllumination_Array3_ShouldDoLessOperations) {
    std::string experimentCode = R"(
    func test() {
        var a = [..3]
        a[2] = 9
        a[0] = 3
        a[1] = 4
        var c = [1, 2, 3]
        print(a)
    }
    {
        test()
    }
    )";

    std::string expectedOutput = "[3, 4, 9]\n";
    std::string notOptimizedOutput = runCode(experimentCode, false);
    std::string optimizedOutput = runCode(experimentCode, true);
    EXPECT_EQ(notOptimizedOutput, expectedOutput);
    EXPECT_EQ(optimizedOutput, expectedOutput);

    size_t operationWithoutOptimizations = runCodeAndGetOperationsCount(experimentCode, false);
    size_t operationWithOptimizations = runCodeAndGetOperationsCount(experimentCode, true);
    EXPECT_LT(operationWithOptimizations, operationWithoutOptimizations);
}

TEST(JITTests, ConstantFolding_VariablesCreating_ShouldHaveLessOperationsCount) {
    std::string experimentCode = R"(
    func test() {
        var a = 1 + 1
        print(a)
    }
    {
        test()
    }
    )";

    size_t operationWithoutOptimizations = runCodeAndGetOperationsCount(experimentCode, false);
    size_t operationWithOptimizations = runCodeAndGetOperationsCount(experimentCode, true);
    EXPECT_LT(operationWithOptimizations, operationWithoutOptimizations);
}

TEST(JITTests, ConstantFolding_If_ShouldHaveLessOperationsCount) {
    std::string experimentCode = R"(
    func test() {
        if (true or false) {
            print(true)
        }
        print(4)
    }
    {
        test()
    }
    )";

    size_t operationWithoutOptimizations = runCodeAndGetOperationsCount(experimentCode, false);
    size_t operationWithOptimizations = runCodeAndGetOperationsCount(experimentCode, true);
    EXPECT_LT(operationWithOptimizations, operationWithoutOptimizations);
}

TEST(JITTests, ConstantFolding_While_ShouldHaveLessOperationsCount) {
    std::string experimentCode = R"(
    func test() {
        while (true or false) {
            return 0
        }
        print(4)
    }
    {
        print(test())
    }
    )";

    size_t operationWithoutOptimizations = runCodeAndGetOperationsCount(experimentCode, false);
    size_t operationWithOptimizations = runCodeAndGetOperationsCount(experimentCode, true);
    EXPECT_LT(operationWithOptimizations, operationWithoutOptimizations);
}

TEST(JITTests, ConstantFolding_For_ShouldHaveLessOperationsCount) {
    std::string experimentCode = R"(
    func test() {
        for (var i = 0; i < 5 * 8 + 1; i = i + 1) {
            print(i)
        }
    }
    {
        test()
    }
    )";

    size_t operationWithoutOptimizations = runCodeAndGetOperationsCount(experimentCode, false);
    size_t operationWithOptimizations = runCodeAndGetOperationsCount(experimentCode, true);
    EXPECT_LT(operationWithOptimizations, operationWithoutOptimizations);
}

TEST(JITTests, ConstantFolding_Return_ShouldHaveLessOperationsCount) {
    std::string experimentCode = R"(
    func test() {
        return 5 * 8 + 1 - 3
    }
    {
        print(test())
    }
    )";

    size_t operationWithoutOptimizations = runCodeAndGetOperationsCount(experimentCode, false);
    size_t operationWithOptimizations = runCodeAndGetOperationsCount(experimentCode, true);
    EXPECT_LT(operationWithOptimizations, operationWithoutOptimizations);

    std::string expectedOutput = "38\n";
    std::string actualOutputWithoutOptimization = runCode(experimentCode, false);
    std::string actualOutputWithOptimization = runCode(experimentCode, true);
    EXPECT_EQ(expectedOutput, actualOutputWithoutOptimization);
    EXPECT_EQ(expectedOutput, actualOutputWithOptimization);
}

TEST(JITTests, ConstantFolding_FunctionCall_ShouldHaveLessOperationsCount) {
    std::string experimentCode = R"(
    func test(a: num, b: num, c: boolean) {
        print(a)
        print(b)
        print(c)
    }
    func b() {
        test(5 * 6 + 1, (4 / 2 + 7) * 10, !true)
        print(1)
    }
    {
        b()
    }
    )";

    size_t operationWithoutOptimizations = runCodeAndGetOperationsCount(experimentCode, false);
    size_t operationWithOptimizations = runCodeAndGetOperationsCount(experimentCode, true);
    EXPECT_LT(operationWithOptimizations, operationWithoutOptimizations);

    std::string expectedOutput = "31\n90\nfalse\n1\n";
    std::string actualOutputWithoutOptimization = runCode(experimentCode, false);
    std::string actualOutputWithOptimization = runCode(experimentCode, true);
    EXPECT_EQ(expectedOutput, actualOutputWithoutOptimization);
    EXPECT_EQ(expectedOutput, actualOutputWithOptimization);
}

TEST(JITTests, ConstantFolding_ArrayCreating_ShouldHaveLessOperationsCount) {
    std::string experimentCode = R"(
    func b() {
        a = [..10 + 6]
        print(__size(a))
    }
    {
        b()
    }
    )";

    size_t operationWithoutOptimizations = runCodeAndGetOperationsCount(experimentCode, false);
    size_t operationWithOptimizations = runCodeAndGetOperationsCount(experimentCode, true);
    EXPECT_LT(operationWithOptimizations, operationWithoutOptimizations);

    std::string expectedOutput = "16\n";
    std::string actualOutputWithoutOptimization = runCode(experimentCode, false);
    std::string actualOutputWithOptimization = runCode(experimentCode, true);
    EXPECT_EQ(expectedOutput, actualOutputWithoutOptimization);
    EXPECT_EQ(expectedOutput, actualOutputWithOptimization);
}

TEST(JITTests, ConstantFolding_ArraySet_ShouldHaveLessOperationsCount) {
    std::string experimentCode = R"(
    func b() {
        a = [..10 + 6]
        a[4] = (5 + 7 * 8) * 2 + 8
        print(a[4])
    }
    {
        b()
    }
    )";

    size_t operationWithoutOptimizations = runCodeAndGetOperationsCount(experimentCode, false);
    size_t operationWithOptimizations = runCodeAndGetOperationsCount(experimentCode, true);
    EXPECT_LT(operationWithOptimizations, operationWithoutOptimizations);

    std::string expectedOutput = "130\n";
    std::string actualOutputWithoutOptimization = runCode(experimentCode, false);
    std::string actualOutputWithOptimization = runCode(experimentCode, true);
    EXPECT_EQ(expectedOutput, actualOutputWithoutOptimization);
    EXPECT_EQ(expectedOutput, actualOutputWithOptimization);
}

TEST(JITTests, ConstantFolding_Print_ShouldHaveLessOperationsCount) {
    std::string experimentCode = R"(
    func b() {
        print((-11 + 3 + 2 * 12) % 5)
    }
    {
        b()
    }
    )";

    size_t operationWithoutOptimizations = runCodeAndGetOperationsCount(experimentCode, false);
    size_t operationWithOptimizations = runCodeAndGetOperationsCount(experimentCode, true);
    EXPECT_LT(operationWithOptimizations, operationWithoutOptimizations);

    std::string expectedOutput = "1\n";
    std::string actualOutputWithoutOptimization = runCode(experimentCode, false);
    std::string actualOutputWithOptimization = runCode(experimentCode, true);
    EXPECT_EQ(expectedOutput, actualOutputWithoutOptimization);
    EXPECT_EQ(expectedOutput, actualOutputWithOptimization);
}

TEST(JITTests, Experiment1) {
    std::string experimentCode = R"(
    func test(in: num) {
        var r = in + 3
        var e = 2 + 4 - 1
        var d = !true or (!true or false) and true or (5 > 3) and (7 < in) or false
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

    size_t operationWithoutOptimizations = runCodeAndGetOperationsCount(experimentCode, false);
    size_t operationWithOptimizations = runCodeAndGetOperationsCount(experimentCode, true);
    EXPECT_LT(operationWithOptimizations, operationWithoutOptimizations);

    std::string expectedOutput = "58\ntrue\n5\n37\n47\n5\n9\n\"finish\"\n";
    std::string actualOutputWithoutOptimization = runCode(experimentCode, false);
    std::string actualOutputWithOptimization = runCode(experimentCode, true);
    EXPECT_EQ(expectedOutput, actualOutputWithoutOptimization);
    EXPECT_EQ(expectedOutput, actualOutputWithOptimization);
}

TEST(JITTests, Experiment2) {
    std::string experimentCode = R"(
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

    size_t operationWithoutOptimizations = runCodeAndGetOperationsCount(experimentCode, false);
    size_t operationWithOptimizations = runCodeAndGetOperationsCount(experimentCode, true);
    EXPECT_LT(operationWithOptimizations, operationWithoutOptimizations);

    std::string expectedOutput = "11\n64\n\"gol\"\n\"finish\"\n";
    std::string actualOutputWithoutOptimization = runCode(experimentCode, false);
    std::string actualOutputWithOptimization = runCode(experimentCode, true);
    EXPECT_EQ(expectedOutput, actualOutputWithoutOptimization);
    EXPECT_EQ(expectedOutput, actualOutputWithOptimization);
}

TEST(JITTests, Experiment3) {
    std::string experimentCode = R"(
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

    size_t operationWithoutOptimizations = runCodeAndGetOperationsCount(experimentCode, false);
    size_t operationWithOptimizations = runCodeAndGetOperationsCount(experimentCode, true);
    EXPECT_LT(operationWithOptimizations, operationWithoutOptimizations);

    std::string expectedOutput = "6\n7\n8\n9\n-3\n-2\n-1\n0\n64\n\"gol\"\n\"finish\"\n";
    std::string actualOutputWithoutOptimization = runCode(experimentCode, false);
    std::string actualOutputWithOptimization = runCode(experimentCode, true);
    EXPECT_EQ(expectedOutput, actualOutputWithoutOptimization);
    EXPECT_EQ(expectedOutput, actualOutputWithOptimization);
}

TEST(JITTests, CasheForFunctions_ShouldSave) {
    std::string experimentCode = R"(
    func red(left: num, right: num) {
        return left + right
    }
    {
        print(red(3, 4))
        print(red(3, 4))
    }
  )";

    size_t operationWithoutOptimizations = runCodeAndGetOperationsCount(experimentCode, false);
    size_t operationWithOptimizations = runCodeAndGetOperationsCount(experimentCode, true);
    EXPECT_LT(operationWithOptimizations, operationWithoutOptimizations);

    std::string expectedOutput = "7\n7\n";
    std::string actualOutputWithoutOptimization = runCode(experimentCode, false);
    std::string actualOutputWithOptimization = runCode(experimentCode, true);
    EXPECT_EQ(expectedOutput, actualOutputWithoutOptimization);
    EXPECT_EQ(expectedOutput, actualOutputWithOptimization);
}

TEST(JITTests, CasheForFunctions_ShouldNotSave_OtherArguments) {
    std::string experimentCode = R"(
    func red(left: num, right: num) {
        return left + right
    }
    {
        print(red(3, 4))
        print(red(3, 5))
    }
  )";

    size_t operationWithoutOptimizations = runCodeAndGetOperationsCount(experimentCode, false);
    size_t operationWithOptimizations = runCodeAndGetOperationsCount(experimentCode, true);
    EXPECT_EQ(operationWithOptimizations, operationWithoutOptimizations);

    std::string expectedOutput = "7\n8\n";
    std::string actualOutputWithoutOptimization = runCode(experimentCode, false);
    std::string actualOutputWithOptimization = runCode(experimentCode, true);
    EXPECT_EQ(expectedOutput, actualOutputWithoutOptimization);
    EXPECT_EQ(expectedOutput, actualOutputWithOptimization);
}

TEST(JITTests, CasheForFunctions_ShouldNotSave_Print) {
    std::string experimentCode = R"(
    func red(left: num, right: num) {
        print(left + right)
        return left + right
    }
    {
        print(red(3, 4))
        print(red(3, 4))
    }
  )";

    size_t operationWithoutOptimizations = runCodeAndGetOperationsCount(experimentCode, false);
    size_t operationWithOptimizations = runCodeAndGetOperationsCount(experimentCode, true);
    EXPECT_EQ(operationWithOptimizations, operationWithoutOptimizations);

    std::string expectedOutput = "7\n7\n7\n7\n";
    std::string actualOutputWithoutOptimization = runCode(experimentCode, false);
    std::string actualOutputWithOptimization = runCode(experimentCode, true);
    EXPECT_EQ(expectedOutput, actualOutputWithoutOptimization);
    EXPECT_EQ(expectedOutput, actualOutputWithOptimization);
}

TEST(JITTests, CasheForFunctions_ShouldNotSave_Print_Hard) {
    std::string experimentCode = R"(
    func red(left: num, right: num) {
        if (left > 3) {
            return 9
        }
        print(left + right)
        return left + right
    }
    {
        print(red(3, 4))
        print(red(3, 4))
    }
  )";

    size_t operationWithoutOptimizations = runCodeAndGetOperationsCount(experimentCode, false);
    size_t operationWithOptimizations = runCodeAndGetOperationsCount(experimentCode, true);
    EXPECT_EQ(operationWithOptimizations, operationWithoutOptimizations);

    std::string expectedOutput = "7\n7\n7\n7\n";
    std::string actualOutputWithoutOptimization = runCode(experimentCode, false);
    std::string actualOutputWithOptimization = runCode(experimentCode, true);
    EXPECT_EQ(expectedOutput, actualOutputWithoutOptimization);
    EXPECT_EQ(expectedOutput, actualOutputWithOptimization);
}

TEST(JITTests, CasheForFunctions_ShouldNotSave_Call) {
    std::string experimentCode = R"(
    func tree() {
        print(5)
    }
    func red(left: num, right: num) {
        tree()
        return left + right
    }
    {
        print(red(3, 4))
        print(red(3, 4))
    }
  )";

    size_t operationWithoutOptimizations = runCodeAndGetOperationsCount(experimentCode, false);
    size_t operationWithOptimizations = runCodeAndGetOperationsCount(experimentCode, true);
    EXPECT_EQ(operationWithOptimizations, operationWithoutOptimizations);

    std::string expectedOutput = "5\n7\n5\n7\n";
    std::string actualOutputWithoutOptimization = runCode(experimentCode, false);
    std::string actualOutputWithOptimization = runCode(experimentCode, true);
    EXPECT_EQ(expectedOutput, actualOutputWithoutOptimization);
    EXPECT_EQ(expectedOutput, actualOutputWithOptimization);
}

TEST(JITTests, SpeedTest_ShoudWorkFaster) {
    std::string experimentCode = R"(
    func test(z: num) {
        var a = 4 + 1
        var b = 3 * 2 + 5 + 9 * 100
        if (z == 10 * 10 * 10 * 10 * 10 * 10) {
            return 0
        }
        test(z + 1)
    }
    {
        test(0)
    }
    )";

    size_t operationWithoutOptimizations = runCodeAndGetOperationsCount(experimentCode, false);
    size_t operationWithOptimizations = runCodeAndGetOperationsCount(experimentCode, true);
    EXPECT_LT(operationWithOptimizations, operationWithoutOptimizations);
}

TEST(JITTests, SpeedTest_ShoudWorkFaster2) {
    std::string experimentCode = R"(
    func red(left: num, right: num) {
        return left + right
    }
    {
        print(red(3, 4))
        print(red(3, 4))
        print(red(3, 4))
    }
    )";

    size_t operationWithoutOptimizations = runCodeAndGetOperationsCount(experimentCode, false);
    size_t operationWithOptimizations = runCodeAndGetOperationsCount(experimentCode, true);
    EXPECT_LT(operationWithOptimizations, operationWithoutOptimizations);
}

TEST(JITTests, SpeedTest_ShoudWorkFaster3) {
    std::string experimentCode = R"(
    func factorial(a: num) {
        if (a == 1 or a == 0) {
            return 1
        }
        return a * factorial(a - 1)
    }
    {
        for(var i = 20; i >= 0; i = i - 1) {
              print(factorial(i))
        }
    }
    )";

    size_t operationWithoutOptimizations = runCodeAndGetOperationsCount(experimentCode, false);
    size_t operationWithOptimizations = runCodeAndGetOperationsCount(experimentCode, true);
    std::cout << operationWithoutOptimizations << " " << operationWithOptimizations << '\n';
    EXPECT_LT(operationWithOptimizations, operationWithoutOptimizations);
}

TEST(JITTests, BehaviorTest_DeadCode_DivOnZero) {
    std::string experimentCode = R"(
    func test() {
        var a = 1 / 0
    }
    {
        test()
    }
    )";

    EXPECT_THROW(runCodeAndGetOperationsCount(experimentCode, false), std::runtime_error);
    EXPECT_THROW(runCodeAndGetOperationsCount(experimentCode, true), std::runtime_error);
}

TEST(JITTests, BehaviorTest_DeadCode_ArrayCreating) {
    std::string experimentCode = R"(
    func b() {
        var a = [..10]
        var vec = [-1, 3, 2.5, 2.4, -5]
    }
    {
        b()
    }
    )";

    EXPECT_NO_THROW(runCodeAndGetOperationsCount(experimentCode, false));
    EXPECT_NO_THROW(runCodeAndGetOperationsCount(experimentCode, true));
}

TEST(JITTests, BehaviorTest_DeadCode_ArrayCreating2) {
    std::string experimentCode = R"(
    func b() {
        var a = [..-10]
        var vec = [-1, 3, 2.5, 2.4, -5]
    }
    {
        b()
    }
    )";

    EXPECT_THROW(runCodeAndGetOperationsCount(experimentCode, false), std::runtime_error);
    EXPECT_THROW(runCodeAndGetOperationsCount(experimentCode, true), std::runtime_error);
}

TEST(JITTests, BehaviorTest_DeadCode_ArrayCreating3) {
    std::string experimentCode = R"(
    func b() {
        var a = [..10]
        var vec = [-1, 3 / 0, 2.5, 2.4, -5]
    }
    {
        b()
    }
    )";

    EXPECT_THROW(runCodeAndGetOperationsCount(experimentCode, false), std::runtime_error);
    EXPECT_THROW(runCodeAndGetOperationsCount(experimentCode, true), std::runtime_error);
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
