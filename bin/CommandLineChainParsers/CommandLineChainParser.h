#pragma once
#include<memory>
#include<vector>
#include"../Commands/Command.h"

class CommandLineChainParser {
public:
    virtual std::unique_ptr<Command> Parse(std::vector<std::string>& request) = 0;

    CommandLineChainParser& AddNextParser(std::unique_ptr<CommandLineChainParser> parser) {
        if (next_parser_ == nullptr) {
            next_parser_ = std::move(parser);
            return *next_parser_;
        }
        return next_parser_->AddNextParser(std::move(parser));
    };

    virtual ~CommandLineChainParser() = default;

protected:
    std::unique_ptr<CommandLineChainParser> next_parser_ = nullptr;
};
