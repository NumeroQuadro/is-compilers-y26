#include "CommandLineChainParsers/CommandLineChainParser.h"
#include "CommandLineChainParsers/GetTimeDecoratorParser.h"
#include "CommandLineChainParsers/RunWithLogsCommandParser.h"
#include "CommandLineChainParsers/RunCommandParser.h"
#include <iostream>
#include <memory>
#include <vector>
#include <string>

int main(int argc, char *argv[]) {
    GetTimeDecoratorParser parser;

    parser
        .AddNextParser(std::make_unique<RunWithLogsCommandParser>())
        .AddNextParser(std::make_unique<RunCommandParser>());

    std::vector<std::string> request(argv, argv + argc);
    auto command = parser.Parse(request);
    if (!command.get())
    {
        return 0;
    }

    command->Execute();
    return 0;
}
