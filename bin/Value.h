#pragma once

#include <stdexcept>
#include <utility>
#include <vector>
#include <variant>

class Value;

class HeapValue {
public:
  virtual ~HeapValue() = default;
};

class ArrayValue final : public HeapValue {
public:
  std::vector<Value> elements;

  explicit ArrayValue(const size_t size): elements(size) {
  }
};

class StringValue : public HeapValue {
public:
  std::string value;
  explicit StringValue(std::string v) : value(std::move(v)) {}
};

enum class ValueType {
  INT,
  FLOAT,
  BOOL,
  REF
};

class Value {
public:
  Value() : _type(ValueType::INT) {
    _data = 0;
  }

  explicit Value(int v) : _type(ValueType::INT) {
    _data = v;
  }

  explicit Value(float v) : _type(ValueType::FLOAT) {
    _data = v;
  }

  explicit Value(bool b) : _type(ValueType::BOOL) {
    _data = b;
  }

  explicit Value(HeapValue *ref) : _type(ValueType::REF) {
    _data = ref;
  }

  [[nodiscard]] int asInt() const {
    if (_type == ValueType::INT) return std::get<int>(_data);
    throw std::runtime_error("Value is not int");
  }

  [[nodiscard]] float asFloat() const {
    if (_type == ValueType::FLOAT) return std::get<float>(_data);
    throw std::runtime_error("Value is not float");
  }

  [[nodiscard]] bool asBool() const {
    if (_type == ValueType::BOOL) return std::get<bool>(_data);
    throw std::runtime_error("Value is not bool");
  }

  [[nodiscard]] HeapValue *asHeapRef() const {
    if (_type == ValueType::REF) return std::get<HeapValue *>(_data);
    throw std::runtime_error("Value is not heap reference");
  }

  [[nodiscard]] std::variant<int, float, bool, HeapValue *> getValue() const {
    return _data;
  }

  [[nodiscard]] ValueType getType() const {
    return _type;
  }

private:
  std::variant<int, float, bool, HeapValue *> _data;
  ValueType _type;
};
