#pragma once
#include <string>
#include <vector>

#include "Value.h"

struct FunctionParam {
  std::string name;
  ValueType type;
};

struct FunctionInfo {
  std::string name;
  std::vector<FunctionParam> params;
  int64_t address;
};
