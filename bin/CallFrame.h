#pragma once
#include "Scope.h"

struct CallFrame {
  int64_t returnIp;
  Scope* prevScope;
  bool hasReturnValue;
};
