#pragma once
#include <FunctionInfo.h>

#include "Scope.h"

struct CallFrame {
  int64_t returnIp{};
  Scope* prevScope{};
  bool hasReturnValue{};
  std::string funcName{};
};
