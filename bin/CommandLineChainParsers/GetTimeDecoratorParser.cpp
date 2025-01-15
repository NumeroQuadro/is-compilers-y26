#include"GetTimeDecoratorParser.h"
#include"../Commands/GetTimeDecorator.h"
#include<algorithm>
#include<iostream>


std::unique_ptr<Command> GetTimeDecoratorParser::Parse(std::vector<std::string>& request) {
    bool is_with_time = std::find(request.begin(), request.end(), "-t") != request.end();
    if (!is_with_time) {
        is_with_time = std::find(request.begin(), request.end(), "--time") != request.end();
    }

    if (!next_parser_) {
        std::cerr << "Unknown command\n";
        return nullptr;
    }

    if (!is_with_time) {
        return next_parser_->Parse(request);
    }

    auto command = std::move(next_parser_->Parse(request));
    if (!command.get())
    {
        return nullptr;
    }
    
    return std::make_unique<GetTimeDecorator>(std::move(command));
}