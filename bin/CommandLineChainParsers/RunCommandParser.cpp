#include"RunCommandParser.h"
#include"../Commands/RunCommand.h"
#include<algorithm>
#include<iostream>

std::unique_ptr<Command> RunCommandParser::Parse(std::vector<std::string>& request) {
    auto pos = std::find(request.begin(), request.end(), "-i");
    if (pos == request.end()) {
        pos = std::find(request.begin(), request.end(), "--input");
    }

    if (pos == request.end() && next_parser_) {
        return next_parser_->Parse(request);
    }

    if (pos == request.end()) {
        std::cerr << "Unknown command\n";
        return nullptr;
    }

    int pos_i = pos - request.begin();
    if (request.size() <= pos_i + 1)
    {
        std::cerr << "Input file not set\n";
        return nullptr;
    }
    
    bool opt = std::find(request.begin(), request.end(), "-O2") == request.end();
    std::string filename(std::move(request[pos_i + 1]));

    return std::make_unique<RunCommand>(opt, std::move(filename));
}