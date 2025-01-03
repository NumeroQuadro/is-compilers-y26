#pragma once

#include <cmath>
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
  DOUBLE,
  BOOL,
  REF
};

class Value {
public:
  Value() : _data(int64_t(0)), _type(ValueType::INT) {}

  explicit Value(int64_t n) : _data(n), _type(ValueType::INT) {}

  explicit Value(double d) : _data(d), _type(ValueType::DOUBLE) {}

  explicit Value(bool b) : _type(ValueType::BOOL) {
    _data = b;
  }

  explicit Value(HeapValue *ref) : _type(ValueType::REF) {
    _data = ref;
  }

  [[nodiscard]] int64_t asInt() const {
    if (_type == ValueType::INT) {
      return std::get<int64_t>(_data);
    }
    if (_type == ValueType::BOOL) {
      return std::get<bool>(_data) ? 1 : 0;
    }
    if (_type == ValueType::DOUBLE) {
      return static_cast<int64_t>(std::get<double>(_data));
    }

    throw std::runtime_error("Value is not int-compatible (REF?)");
  }

  [[nodiscard]]double asDouble() const {
    if(_type == ValueType::DOUBLE) {
      return std::get<double>(_data);
    }
    if (_type == ValueType::INT) {
      return static_cast<double>(std::get<int64_t>(_data));
    }
    if (_type == ValueType::BOOL) {
      return std::get<bool>(_data) ? 1 : 0;
    }

    throw std::runtime_error("Value is not double-compatible (REF?)");
  }

  [[nodiscard]] bool asBool() const {
    if (_type == ValueType::BOOL) {
      return std::get<bool>(_data);
    }
    if (_type == ValueType::INT) {
      return std::get<int64_t>(_data) != 0;
    }
    if(_type == ValueType::DOUBLE) {
      return std::fabs(std::get<double>(_data)) > 1e-9;
    }

    throw std::runtime_error("Value is not bool-compatible (REF?)");
  }

  [[nodiscard]] HeapValue *asHeapRef() const {
    if (_type == ValueType::REF) {
      return std::get<HeapValue *>(_data);
    }

    throw std::runtime_error("Value is not heap reference");
  }

  [[nodiscard]] std::variant<int64_t, double, bool, HeapValue *> getValue() const {
    return _data;
  }

  [[nodiscard]] ValueType getType() const {
    return _type;
  }

  friend Value operator+(const Value &lhs, const Value &rhs) {
    if (lhs._type == ValueType::REF && rhs._type == ValueType::REF) {
      throw std::runtime_error("Cannot add references (REF)");
    }

    const bool useDouble = (lhs._type == ValueType::DOUBLE || rhs._type == ValueType::DOUBLE);

    if (useDouble) {
      const double sum = lhs.asDouble() + rhs.asDouble();
      return Value(sum);
    }

    const int64_t sum = lhs.asInt() + rhs.asInt();
    return Value(sum);
  }

  friend Value operator-(const Value &lhs, const Value &rhs) {
    if (lhs._type == ValueType::REF && rhs._type == ValueType::REF) {
      throw std::runtime_error("Cannot add references (REF)");
    }

    const bool useDouble = (lhs._type == ValueType::DOUBLE || rhs._type == ValueType::DOUBLE);

    if (useDouble) {
      const double diff = lhs.asDouble() - rhs.asDouble();
      return Value(diff);
    }

    const int64_t diff = lhs.asInt() - rhs.asInt();
    return Value(diff);
  }

  friend Value operator*(const Value &lhs, const Value &rhs) {
    if(lhs._type == ValueType::REF || rhs._type == ValueType::REF) {
      throw std::runtime_error("Cannot mul references (REF)");
    }
    bool useDouble = (lhs._type == ValueType::DOUBLE || rhs._type == ValueType::DOUBLE);

    if(useDouble) {
      const double prod = lhs.asDouble() * rhs.asDouble();
      return Value(prod);
    }

    const int64_t prod = lhs.asInt() * rhs.asInt();
    return Value(prod);
  }

  friend Value operator/(const Value &lhs, const Value &rhs) {
    if(lhs._type == ValueType::REF || rhs._type == ValueType::REF) {
      throw std::runtime_error("Cannot div references (REF)");
    }
    const bool useDouble = (lhs._type == ValueType::DOUBLE || rhs._type == ValueType::DOUBLE);

    if(rhs.asDouble() == 0.0) {
      throw std::runtime_error("Division by zero");
    }

    if(useDouble) {
      const double quotient = lhs.asDouble() / rhs.asDouble();
      return Value(quotient);
    }

    const int64_t b = rhs.asInt();
    if(b == 0) {
      throw std::runtime_error("Division by zero (int)");
    }
    const int64_t a = lhs.asInt();
    return Value(a / b);
  }

  friend Value operator%(const Value &lhs, const Value &rhs) {
    if(lhs._type == ValueType::REF || rhs._type == ValueType::REF) {
      throw std::runtime_error("Cannot div with remainder references (REF)");
    }
    const bool useDouble = (lhs._type == ValueType::DOUBLE || rhs._type == ValueType::DOUBLE);

    if(rhs.asDouble() == 0.0) {
      throw std::runtime_error("Division by zero");
    }

    if(useDouble) {
      throw std::runtime_error("Work only with int");
    }

    const int64_t b = rhs.asInt();
    if(b == 0) {
      throw std::runtime_error("Division by zero (int)");
    }
    const int64_t a = lhs.asInt();
    return Value(a % b);
  }

  friend bool operator==(const Value &lhs, const Value &rhs) {
    if(lhs._type == ValueType::REF && rhs._type == ValueType::REF) {
      return lhs.asHeapRef() == rhs.asHeapRef();
    }
    if(lhs._type == ValueType::REF || rhs._type == ValueType::REF) {
      return false;
    }

    const bool useDouble = (lhs._type == ValueType::DOUBLE || rhs._type == ValueType::DOUBLE);
    if(useDouble) {
      return (lhs.asDouble() == rhs.asDouble());
    }

    return (lhs.asInt() == rhs.asInt());
  }

  friend bool operator!=(const Value &lhs, const Value &rhs) {
    return !(lhs == rhs);
  }

  friend bool operator<(const Value &lhs, const Value &rhs) {
    if(lhs._type == ValueType::REF && rhs._type == ValueType::REF) {
      return lhs.asHeapRef() < rhs.asHeapRef();
    }
    if(lhs._type == ValueType::REF || rhs._type == ValueType::REF) {
      return false;
    }

    const bool useDouble = (lhs._type == ValueType::DOUBLE || rhs._type == ValueType::DOUBLE);
    if(useDouble) {
      return (lhs.asDouble() < rhs.asDouble());
    }

    return (lhs.asInt() < rhs.asInt());
  }

  friend bool operator<=(const Value &lhs, const Value &rhs) {
    return (lhs < rhs || lhs == rhs);
  }

  friend bool operator>(const Value &lhs, const Value &rhs) {
    return !(lhs <= rhs);
  }

  friend bool operator>=(const Value &lhs, const Value &rhs) {
    return !(lhs < rhs);
  }
private:
  std::variant<int64_t, double, bool, HeapValue *> _data;
  ValueType _type;
};

// num, string, array, bool
// Num + Num int64 -> int128 -> string
// int, float, string, array, bool
