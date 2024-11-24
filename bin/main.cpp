#include "antlr4-runtime.h"
#include "GrammarLexer.h"
#include "GrammarParser.h"
#include "YalLLVM.h"

#include <filesystem>
#include <iostream>
#include <fstream>
#include <string>

int main()
{
  std::cout << "Current working directory: "
            << std::filesystem::current_path() << std::endl;

  std::cout << "./src/example.txt";

  std::string program = R"(
  
  42
  
  )";

  YalLLVM vm;
  // vm.exec(program);

  std::ifstream stream("./src/example.txt");
  if (!stream)
  {
    std::cerr << "Cannot open input file\n";
    return 1;
  }

  antlr4::ANTLRInputStream input(stream);
  GrammarLexer lexer(&input);
  antlr4::CommonTokenStream tokens(&lexer);
  GrammarParser parser(&tokens);
  GrammarParser::ProgContext *tree = parser.prog();

  // std::cout << "Parsed: " << tree->toStringTree(&parser) << std::endl;
  return 0;
}
