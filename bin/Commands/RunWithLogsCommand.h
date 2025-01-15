#pragma once
#include"Command.h"
#include<string>

class RunWithLogsCommand : public Command  {
public:
    RunWithLogsCommand(bool opt, std::string filename);

    void Execute() override;

private:
    bool opt_;
    std::string filename_;
};
