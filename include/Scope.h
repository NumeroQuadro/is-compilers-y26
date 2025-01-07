#pragma once
#include <string>
#include <unordered_map>

#include "Value.h"

struct  Scope {
  std::unordered_map<std::string, Value> values;
  Scope *previous_ = nullptr;

  std::vector<HeapValue*> objectsInThisScope;
public:
  Scope(Scope *previous) : previous_(previous) {}

  bool isClosed = false;

  bool findVar(const std::string& name, Value& out) const;
  bool setVar(const std::string &name, const Value &val);
  void createVar(const std::string &name, const Value &val);
};
