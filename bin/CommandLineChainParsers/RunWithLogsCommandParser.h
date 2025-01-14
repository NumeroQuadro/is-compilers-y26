#pragma once
#include"CommandLineChainParser.h"

class RunWithLogsCommandParser : public CommandLineChainParser {
public:
    std::unique_ptr<Command> Parse(std::vector<std::string>& request) override;
};