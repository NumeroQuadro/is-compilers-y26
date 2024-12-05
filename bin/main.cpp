#include <any>
#include <iostream>
#include <memory>
#include <typeinfo>
#include <unordered_map>

#include "GrammarBaseVisitor.h"
#include "GrammarLexer.h"
#include "GrammarParser.h"
#include "antlr4-runtime.h"

class SimpleValue {
   public:
    int value;
    SimpleValue(int val = 0) : value(val) {}
};

class CustomVisitor : public GrammarBaseVisitor {
   private:
    std::unordered_map<std::string, std::any> variables;

   public:
    // Evaluate expressions recursively
    std::any visitExpression(GrammarParser::ExpressionContext *ctx) override {
        std::cout << "Evaluating expression" << std::endl;

        // If the expression is a comparison, handle that first
        if (ctx->comparison()) { return visit(ctx->comparison()); }

        // Handle logical operations ('and', 'or')
        if (ctx->expression().size() == 2) {
            auto left = visit(ctx->expression(0));
            auto right = visit(ctx->expression(1));

            // For now, we just assume it's an 'or' operation with integers
            return SimpleValue(std::any_cast<SimpleValue>(left).value ||
                               std::any_cast<SimpleValue>(right).value);
        }

        return visitChildren(ctx);
    }

    // Handle comparisons like ==, <, >, etc.
    std::any visitComparison(GrammarParser::ComparisonContext *ctx) override {
        std::cout << "Evaluating comparison" << std::endl;

        // Visit the left and right terms
        auto left = visit(ctx->term(0));
        auto right = visit(ctx->term(1));

        // Get the comparison operator text (from the vector of operators)
        std::cout << "size of vector in visitComparison method: "
                  << ctx->comparisonOperator().size() << std::endl;
        std::string op = ctx->comparisonOperator().empty()
                             ? ""
                             : ctx->comparisonOperator(0)->getText();  // Get the first operator

        // Cast the left and right values to SimpleValue
        int leftVal = std::any_cast<SimpleValue>(left).value;
        int rightVal = std::any_cast<SimpleValue>(right).value;

        bool result = false;

        // Evaluate the comparison based on the operator
        if (op == "==") {
            result = (leftVal == rightVal);
        } else if (op == "<") {
            result = (leftVal < rightVal);
        } else if (op == ">") {
            result = (leftVal > rightVal);
        } else if (op == "<>") {
            result = (leftVal != rightVal);
        } else if (op == "<=") {
            result = (leftVal <= rightVal);
        } else if (op == ">=") {
            result = (leftVal >= rightVal);
        }

        // Return the result as a SimpleValue (1 or 0)
        return SimpleValue(result ? 1 : 0);  // Return 1 for true, 0 for false
    }

    // Handle terms, which are simple numbers or identifiers
    std::any visitTerm(GrammarParser::TermContext *ctx) override {
        if (ctx->IDENTIFIER()) {
            std::string id = ctx->IDENTIFIER()->getText();
            if (variables.find(id) != variables.end()) {
                return variables[id];  // Return variable value
            } else {
                std::cerr << "Error: Undefined variable " << id << std::endl;
                return SimpleValue(0);
            }
        } else if (ctx->NUMBER()) {
            return SimpleValue(
                std::stoi(ctx->NUMBER()->getText()));  // Convert number to SimpleValue
        }

        return visitChildren(ctx);
    }

    // Handle assignment, storing the value in the variables map
    std::any visitAssignment(GrammarParser::AssignmentContext *ctx) override {
        std::cout << "from visitAssignment: " << ctx->IDENTIFIER()->getText() << std::endl;
        std::string identifier = ctx->IDENTIFIER()->getText();
        std::any value = visit(ctx->expression());
        variables[identifier] = value;  // Store the evaluated value in the map
        return visitChildren(ctx);
    }

    // Handle variable declarations (like 'num x = 42')
    std::any visitVariableDeclaration(GrammarParser::VariableDeclarationContext *ctx) override {
        std::cout << "from visitVariableDeclaration: " << ctx->IDENTIFIER()->getText() << std::endl;
        std::string identifier = ctx->IDENTIFIER()->getText();
        auto value = visit(ctx->expression());
        variables[identifier] = value;  // Initialize variable with value
        return visitChildren(ctx);
    }

    // Handle type declarations (for now, only 'num' type)
    std::any visitType(GrammarParser::TypeContext *ctx) override {
        std::cout << "from visitType: " << ctx->getText() << std::endl;
        return visitChildren(ctx);
    }
};

int main() {
    std::string program = R"(
    42
    )";

    std::string input_file = "src/example.txt";
    std::ifstream stream(input_file);
    if (!stream) {
        std::cerr << "Cannot open input file!: " << input_file << std::endl;
        return 1;
    }

    antlr4::ANTLRInputStream input(stream);
    GrammarLexer lexer(&input);
    antlr4::CommonTokenStream tokens(&lexer);
    GrammarParser parser(&tokens);
    GrammarParser::ProgContext *tree = parser.prog();

    CustomVisitor visitor;
    visitor.visit(tree);  // Process the parse tree

    std::cout << "Parsed Tree: " << tree->toStringTree(&parser) << std::endl;

    return 0;
}
