#include "Value.h"

#include <stdexcept>
#include <utility>

std::string Value::toString() const {
  switch (type) {
    case Type::INT:    return std::to_string(asInt);
    case Type::FLOAT:  return std::to_string(asFloat);
    case Type::STRING: return asString;
    case Type::BOOL:   return (asBool ? "true" : "false");
    case Type::VECTOR: {
      std::string result = "[";
      for (size_t i = 0; i < asVector.size(); i++) {
        result += asVector[i].toString();
        if (i + 1 < asVector.size()) result += ", ";
      }
      result += "]";
      return result;
    }
  }
  return "unknown";
}

Value Value::makeInt(int v) {
  return Value(v);
}

Value Value::makeFloat(float v) {
  return Value(v);
}

Value Value::makeBool(bool b) {
  return Value(b);
}

Value Value::makeString(std::string s) {
  return Value(std::move(s));
}

Value Value::makeVector(std::vector<Value> v) {
  return Value(std::move(v));
}

Value Value::operator+(const Value &rhs) const {
  if (this->type != Type::INT && rhs.type != Type::INT) {
    throw std::invalid_argument("operator+: type mismatch");
  }

  return makeInt(this->asInt + rhs.asInt);
}

Value Value::operator*(const Value &rhs) const {
  if (this->type != Type::INT && rhs.type != Type::INT) {
    throw std::invalid_argument("operator*: type mismatch");
  }

  return makeInt(this->asInt * rhs.asInt);
}

bool Value::operator>(const Value &rhs) const {
  if (this->type != Type::INT && rhs.type != Type::INT) {
    throw std::invalid_argument("operator*: type mismatch");
  }

  return this->asInt > rhs.asInt;
}
