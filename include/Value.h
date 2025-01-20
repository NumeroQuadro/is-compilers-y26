#pragma once

#include <stdexcept>
#include <utility>
#include <vector>
#include <variant>
#include <memory>

class Value;

class HeapValue {
public:
  bool marked = false;

  virtual ~HeapValue() = default;

  virtual void markChildren() {
  }

    virtual std::unique_ptr<HeapValue> clone() const = 0;
};

class ArrayValue final : public HeapValue {
public:
  std::vector<Value> elements;

  std::vector<size_t> refIndices;

  explicit ArrayValue(const size_t size): elements(size) {
  }

  Value getValue(size_t index) const;

  void setValue(size_t index, const Value& value);

  void pushValue(const Value& value);

  void markChildren() override;

    std::unique_ptr<HeapValue> clone() const override {
        auto copy = std::make_unique<ArrayValue>(elements.size());
        copy->elements = elements;
        copy->refIndices = refIndices;
        return copy;
    }
};

class StringValue final : public HeapValue {
public:
  std::string value;

  explicit StringValue(std::string v) : value(std::move(v)) {
  }

    std::unique_ptr<HeapValue> clone() const override {
        return std::make_unique<StringValue>(value);
    }
};

enum class ValueType {
  INT,
  DOUBLE,
  BOOL,
  REF
};

class Value {
public:
  Value() : _data(static_cast<int64_t>(0)), _type(ValueType::INT) {
  }

  explicit Value(int64_t n) : _data(n), _type(ValueType::INT) {
  }

  explicit Value(double d) : _data(d), _type(ValueType::DOUBLE) {
  }

  explicit Value(bool b) : _type(ValueType::BOOL) {
    _data = b;
  }

  explicit Value(HeapValue *ref) : _data(ref), _type(ValueType::REF) {
  }

  [[nodiscard]] int64_t asInt() const;

  [[nodiscard]] double asDouble() const;

  [[nodiscard]] bool asBool() const;

  [[nodiscard]] HeapValue *asHeapRef() const;

  [[nodiscard]] std::variant<int64_t, double, bool, HeapValue *> getValue() const;

  [[nodiscard]] ValueType getType() const;

  friend Value operator+(const Value &lhs, const Value &rhs);

  friend Value operator-(const Value &lhs, const Value &rhs);

  friend Value operator*(const Value &lhs, const Value &rhs);

  friend Value operator/(const Value &lhs, const Value &rhs);

  friend Value operator%(const Value &lhs, const Value &rhs);

  friend bool operator==(const Value &lhs, const Value &rhs);

  friend bool operator!=(const Value &lhs, const Value &rhs);

  friend bool operator<(const Value &lhs, const Value &rhs);

  friend bool operator<=(const Value &lhs, const Value &rhs);

  friend bool operator>(const Value &lhs, const Value &rhs);

  friend bool operator>=(const Value &lhs, const Value &rhs);

  friend bool operator&&(const Value &lhs, const Value &rhs);

  friend bool operator||(const Value &lhs, const Value &rhs);

  friend  Value operator!(const Value& val);

  friend Value operator-(const Value& val);

  bool isHeapRef() const;

    static std::string valueTypeToStr(ValueType type) {
        if (type == ValueType::INT) {
            return "INT";
        } else if (type == ValueType::BOOL) {
            return "BOOL";
        } else if (type == ValueType::DOUBLE) {
            return "DOUBLE";
        } else {
            return "REF";
        }
    }

    std::string toStr();
private:
  std::variant<int64_t, double, bool, HeapValue *> _data;
  ValueType _type;
};

// num, string, array, bool
// Num + Num int64 -> int128 -> string
// int, float, string, array, bool
