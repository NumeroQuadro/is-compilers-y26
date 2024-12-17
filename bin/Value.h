#pragma once

#include <utility>
#include <vector>
#include <string>

struct Value {
  enum class Type {
    INT,
    FLOAT,
    STRING,
    BOOL,
    VECTOR
  };

  Type type;
  int asInt;
  float asFloat;
  std::string asString;
  bool asBool;
  std::vector<Value> asVector;

  explicit Value()
    : type(Type::INT), asInt(0), asFloat(0.0f), asBool(false) {
  }

  explicit Value(int v)
    : type(Type::INT), asInt(v), asFloat(0.0f), asBool(false) {
  }

  explicit Value(float f)
    : type(Type::FLOAT), asInt(0), asFloat(f), asBool(false) {
  }

  explicit Value(bool b)
    : type(Type::BOOL), asInt(0), asFloat(0.0f), asBool(b) {
  }

  explicit Value(std::string  s)
    : type(Type::STRING), asInt(0), asFloat(0.0f), asString(std::move(s)), asBool(false) {
  }

  explicit Value(std::vector<Value> v)
    : type(Type::VECTOR), asInt(0), asFloat(0.0f), asBool(false), asVector(std::move(v)) {
  }

  std::string toString() const;

  static Value makeInt(int v);
  static Value makeFloat(float v);
  static Value makeBool(bool b);
  static Value makeString(std::string  s);
  static Value makeVector(std::vector<Value> v);

  Value operator+(const Value& rhs) const;
  Value operator*(const Value& rhs) const;
  bool operator>(const Value& rhs) const;
};
