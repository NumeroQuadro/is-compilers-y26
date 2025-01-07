#pragma once
#include <string>
#include <vector>

#include "Value.h"

struct FunctionParam final {
  std::string name;
  ValueType type;
};

struct FunctionInfo final {
  std::string name;
  std::vector<FunctionParam> params;
  int64_t address;
};
