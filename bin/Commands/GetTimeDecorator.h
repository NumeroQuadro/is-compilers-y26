#include"Command.h"
#include<memory>

class GetTimeDecorator : public Command {
public:
    GetTimeDecorator(std::unique_ptr<Command> command);

    void Execute() override;

private:
    std::unique_ptr<Command> command_;
};