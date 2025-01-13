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
#include <fstream>
#include <cxxopts.hpp>
#include "antlr4-runtime.h"
#include "GarbageCollector.h"
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
     GarbageCollector gc;

     VirtualMachine vm(visitor.code, visitor.functionTable, &gc, visitor.startPos);
     //vm.optimize(true);
     vm.run();

   auto end = high_resolution_clock::now();
   auto duration = duration_cast<milliseconds>(end - start);

   std::cout << "Execution time: " << duration.count() << " ms" << std::endl;
 }


int main(int argc, char* argv[]) {
    cxxopts::Options options("YalVM", "yet another language virtual machine");

    bool opt;
    std::string filename;
    options.add_options()
        ("h,help", "Show help")
        ("O2", "Optimization", cxxopts::value(opt))
        ("i,input", "Input file", cxxopts::value(filename));

      try {
        auto result = options.parse(argc, argv);

        if (result.count("help")) {
            std::cout << options.help() << "\n";
            return 0;
        }

        if (!result.count("input")) {
            std::cerr << "Error: option -i, --input not set.\n";
            std::cerr << options.help() << "\n";
            return 1;
        }

    } catch (const cxxopts::OptionException& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Error: File doesn\'t exist\n";
        return 1;
    }
    

    antlr4::ANTLRInputStream inputStream(file);
    GrammarLexer lexer(&inputStream);
    antlr4::CommonTokenStream tokens(&lexer);
    GrammarParser parser(&tokens);

    GrammarParser::ScriptContext *tree = parser.script();

    GrammarASTInterpreter visitor;
    visitor.visit(tree);
//    // visitor.toFile("test.txt");
//    //
//    // VirtualMachine vm;
//    // vm.optimize(true);
//    // vm.fromFile("test.txt");
//    // auto code = vm.getInstructions();
//    // for (int i = 0; i < code.size(); i++) {
//    //     if (code[i] != visitor.code[i]) {
//    //         std::cout << "Error in instruction " + std::to_string(i) + "\n";
//    //         std::cout << code[i].toStr() << "\n";
//    //         std::cout << visitor.code[i].toStr() << "\n";
//    //     }
//    // }
//    // vm.run();
//
  GarbageCollector gc;

  VirtualMachine vm(visitor.code, visitor.functionTable, &gc, visitor.startPos);
  vm.optimize(opt);
  vm.run();

    // getTimeScoreOfLanguage(experiment5);

    return 0;
}
