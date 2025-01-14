#include "RunCommand.h"
#include <iostream>
#include <fstream>
#include "antlr4-runtime.h"
#include "GarbageCollector.h"
#include "GrammarLexer.h"
#include "GrammarParser.h"
#include "GrammarASTInterpreter.h"
#include "VirtualMachine.h"

RunCommand::RunCommand(bool opt, std::string filename)
    : opt_(opt),
      filename_(std::move(filename)) {}

void RunCommand::Execute() {
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
    GarbageCollector gc;

    VirtualMachine vm(visitor.code, visitor.functionTable, &gc, visitor.startPos);
    vm.optimize(opt_);
    vm.run();
}