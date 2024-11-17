#include "YalLLVM.h"
#include <string>

int main() {
    std::string program = R"(
    
    42
    
    )";

    YalLLVM vm;
    vm.exec(program);

    return 0;
}
