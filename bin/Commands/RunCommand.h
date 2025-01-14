#pragma once
#include"Command.h"
#include<string>

class RunCommand : public Command  {
public:
    RunCommand(bool opt, std::string filename);

    void Execute() override;

private:
    bool opt_;
    std::string filename_;
};
