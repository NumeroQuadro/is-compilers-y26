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

int main(int argc, char *argv[]) {
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
