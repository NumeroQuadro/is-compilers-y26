#include "RunWithLogsCommand.h"
#include <iostream>
#include <fstream>
#include "antlr4-runtime.h"
#include "GarbageCollector.h"
#include "GrammarLexer.h"
#include "GrammarParser.h"
#include "GrammarASTInterpreter.h"
#include "VirtualMachine.h"

RunWithLogsCommand::RunWithLogsCommand(bool opt, std::string filename)
    : opt_(opt),
      filename_(std::move(filename)) {}

void RunWithLogsCommand::Execute() {
    std::ifstream file(filename_);
    if (!file.is_open())
    {
        std::cerr << "Error: File doesn\'t exist\n";
        return;
    }

    antlr4::ANTLRInputStream inputStream(file);
    GrammarLexer lexer(&inputStream);
    antlr4::CommonTokenStream tokens(&lexer);
    GrammarParser parser(&tokens);

    GrammarParser::ScriptContext *tree = parser.script();

    GrammarASTInterpreter visitor;
    visitor.visit(tree);
    visitor.toFile("yal_bytecode.txt");
    
    GarbageCollector gc;

    VirtualMachine vm(visitor.code, visitor.functionTable, &gc, visitor.startPos);
    vm.optimize(opt_);
    vm.fromFile("yal_bytecode.txt");
    auto code = vm.getInstructions();
    for (int i = 0; i < code.size(); i++) {
        if (code[i] != visitor.code[i]) {
            std::cout << "Error in instruction " + std::to_string(i) + "\n";
            std::cout << code[i].toStr() << "\n";
            std::cout << visitor.code[i].toStr() << "\n";
        }
    }
    vm.run();
}