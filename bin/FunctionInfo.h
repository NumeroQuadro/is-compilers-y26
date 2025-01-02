#pragma once
#include <string>
#include <vector>

struct FunctionInfo {
  std::string name;
  std::vector<std::string> paramNames;
  int64_t address;
};
