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

struct FunctionCacheParams final {
    std::string name;
    std::vector<std::pair<std::string, Value>> params;

    std::string toStr() {
        std::string str = name;
        for (int i = 0; i < params.size(); i++) {
            str += params[i].first + params[i].second.toStr() + ";";
        }
        return str;
    }
};