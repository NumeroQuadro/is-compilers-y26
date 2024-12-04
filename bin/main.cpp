#include <any>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <typeinfo>

#include "GrammarBaseVisitor.h"
#include "GrammarLexer.h"
#include "GrammarParser.h"
#include "YalLLVM.h"
#include "antlr4-runtime.h"

class SimpleValue {
   public:
    int *value;
};

class CustomVisitor : public GrammarBaseVisitor {
   public:
    std::any visitAssignment(GrammarParser::AssignmentContext *ctx) override {
        std::cout << "from visitAssignment: " << ctx->IDENTIFIER()->getText() << std::endl;
        visit(ctx->expression());
        return visitChildren(ctx);
    }

    std::any visitStatement(GrammarParser::StatementContext *ctx) override {
        std::cout << "from visitStatement" << std::endl;  // Added logging
        return visitChildren(ctx);
    }

    std::any visitIfStatement(GrammarParser::IfStatementContext *ctx) override {
        std::cout << "from visitIfStatement" << std::endl;  // Added logging

        return visitChildren(ctx);
    }

    std::any visitElifStatement(GrammarParser::ElifStatementContext *ctx) override {
        std::cout << "from visitElifStatement" << std::endl;  // Added logging

        return visitChildren(ctx);
    }

    std::any visitElseStatement(GrammarParser::ElseStatementContext *ctx) override {
        std::cout << "from visitElseStatement" << std::endl;  // Added logging

        return visitChildren(ctx);
    }

    std::any visitBlock(GrammarParser::BlockContext *ctx) override {
        std::cout << "from visitBlock" << std::endl;  // Added logging

        return visitChildren(ctx);
    }

    std::any visitWhileStatement(GrammarParser::WhileStatementContext *ctx) override {
        std::cout << "from visitWhileStatement" << std::endl;  // Added logging

        return visitChildren(ctx);
    }

    std::any visitForStatement(GrammarParser::ForStatementContext *ctx) override {
        std::cout << "from visitForStatement" << std::endl;  // Added logging
        return visitChildren(ctx);
    }

    std::any visitExpression(GrammarParser::ExpressionContext *ctx) override {
        std::cout << "from visitExpression" << std::endl;  // Added logging
        return visitChildren(ctx);
    }

    std::any visitComparison(GrammarParser::ComparisonContext *ctx) override {
        std::cout << "from visitComparison" << std::endl;  // Added logging
        return visitChildren(ctx);
    }

    std::any visitComparisonOperator(GrammarParser::ComparisonOperatorContext *ctx) override {
        std::cout << "from visitComparisonOperator" << std::endl;  // Added logging
        return visitChildren(ctx);
    }

    std::any visitTerm(GrammarParser::TermContext *ctx) override {
        std::cout << "from visitTerm" << std::endl;  // Added logging
        return visitChildren(ctx);
    }

    std::any visitVariableDeclaration(GrammarParser::VariableDeclarationContext *ctx) override {
        std::cout << "from visitVariableDeclaration: " << ctx->IDENTIFIER()->getText()
                  << std::endl;  // Example log with identifier
        return visitChildren(ctx);
    }

    std::any visitType(GrammarParser::TypeContext *ctx) override {
        std::cout << "from visitType: " << ctx->getText()
                  << std::endl;  // Example log with type text
        return visitChildren(ctx);
    }
};

int main() {
    std::cout << "./src/example.txt";

    std::string program = R"(
  
  42
  
  )";

    YalLLVM vm;
    // vm.exec(program);

    std::ifstream stream("./src/example.txt");
    if (!stream) {
        std::cerr << "Cannot open input file\n";
        return 1;
    }

    antlr4::ANTLRInputStream input(stream);
    GrammarLexer lexer(&input);
    antlr4::CommonTokenStream tokens(&lexer);
    GrammarParser parser(&tokens);
    GrammarParser::ProgContext *tree = parser.prog();

    CustomVisitor visitor;
    visitor.visit(tree);

    std::cout << "Parsed Tree: " << tree->toStringTree(&parser) << std::endl;

    return 0;
}
