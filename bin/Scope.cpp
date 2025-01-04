#include "Scope.h"

bool Scope::findVar(const std::string &name, Value &out) const {
  const auto it = values.find(name);
  if (it != values.end()) {
    out = it->second;
    return true;
  }
  if (previous_) return previous_->findVar(name, out);

  return false;
}

bool Scope::setVar(const std::string &name, const Value &val) {
  const auto it = values.find(name);
  if (it != values.end()) {
    it->second = val;
    return true;
  }
  if (previous_) return previous_->setVar(name, val);

  return false;
}

void Scope::createVar(const std::string &name, const Value &val) {
  values[name] = val;
}
