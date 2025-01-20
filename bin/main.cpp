#include "CommandLineChainParsers/CommandLineChainParser.h"
#include "CommandLineChainParsers/GetTimeDecoratorParser.h"
#include "CommandLineChainParsers/RunWithLogsCommandParser.h"
#include "CommandLineChainParsers/RunCommandParser.h"
#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <algorithm>

const std::string help_description = R"(
Usage: yal [OPTIONS]

A brief description of what myapp does.

Options:
-i, --input <file>     Specify the input file to process.
-l, --logs             Write bytecode in yal_bytecode.txt and show logs
-t, --time             Show execution time.
-O2                    Add optimization
-h, --help             Display this help message

Examples:
yal --input program.yal
yal -O2 --time --logs --input program.yal
)";


#include <string>
#include <sstream>
#include <iostream>
#include "antlr4-runtime.h"
#include "GarbageCollector.h"
#include "GrammarLexer.h"
#include "GrammarParser.h"
#include "GrammarASTInterpreter.h"
#include "VirtualMachine.h"
int test() {
    std::string codeStr = R"(
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

    antlr4::ANTLRInputStream inputStream(codeStr);
    GrammarLexer lexer(&inputStream);
    antlr4::CommonTokenStream tokens(&lexer);
    GrammarParser parser(&tokens);

    GrammarParser::ScriptContext *tree = parser.script();

    GrammarASTInterpreter visitor;
    visitor.visit(tree);

    GarbageCollector gc;

    for (int i = 0; i < visitor.code.size(); i++) {
        std::cout << visitor.code[i].toStr() << '\n';
    }

    VirtualMachine vm(visitor.code, visitor.functionTable, &gc, visitor.startPos);
    vm.optimize(true);
    vm.run();

    return 0;
}

int main(int argc, char *argv[]) {
    //test();
    GetTimeDecoratorParser parser;

    parser
        .AddNextParser(std::make_unique<RunWithLogsCommandParser>())
        .AddNextParser(std::make_unique<RunCommandParser>());

    std::vector<std::string> request(argv, argv + argc);

    if(std::find(request.begin(), request.end(), "--help") != request.end() &&
       std::find(request.begin(), request.end(), "-h") != request.end()) {
        std::cerr << help_description;
        return 0;
    }

    auto command = parser.Parse(request);
    if (!command.get())
    {
        std::cerr << help_description;
        return 0;
    }

    command->Execute();
    return 0;
}
