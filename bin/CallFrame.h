#pragma once
#include "Scope.h"

struct CallFrame {
  int returnIp;
  Scope* prevScope;
};
