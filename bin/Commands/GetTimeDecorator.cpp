#include"GetTimeDecorator.h"
#include<chrono>
#include<iostream>

GetTimeDecorator::GetTimeDecorator(std::unique_ptr<Command> command) 
    : command_(std::move(command)) {}

void GetTimeDecorator::Execute() {
    using namespace std::chrono;
    auto start = high_resolution_clock::now();

    command_->Execute();

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start);

    std::cout << "Execution time: " << duration.count() << " ms" << std::endl;
}