#include "Value.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <strings.h>

Value ArrayValue::getValue(const size_t index) const {
  if (index >= elements.size()) {
    throw std::out_of_range("Value index out of range");
  }

  return elements[index];
}

void ArrayValue::setValue(const size_t index, const Value &value) {
  if (index >= elements.size()) {
    throw std::runtime_error("Index out of range");
  }
  Value &oldVal = elements[index];
  const bool wasRef = oldVal.isHeapRef();
  const bool isRef = value.isHeapRef();

  oldVal = value;
  if (wasRef && !isRef) {
    if (const auto it = std::find(refIndices.begin(), refIndices.end(), index); it != refIndices.end()) {
      refIndices.erase(it);
    }
  } else if (!wasRef && isRef) {
    refIndices.emplace_back(index);
  }
}

void ArrayValue::pushValue(const Value &value) {
  elements.emplace_back(value);
  if (value.isHeapRef()) {
    refIndices.emplace_back(elements.size() - 1);
  }
}

void ArrayValue::markChildren() {
  for (const auto &index: refIndices) {
    HeapValue *ref = elements[index].asHeapRef();
    ref->marked = true;
    ref->markChildren();
  }
}

int64_t Value::asInt() const {
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

double Value::asDouble() const {
  if (_type == ValueType::DOUBLE) {
    return std::get<double>(_data);
  }
  if (_type == ValueType::INT) {
    return static_cast<double>(std::get<int64_t>(_data));
  }
  if (_type == ValueType::BOOL) {
    return std::get<bool>(_data) ? 1.0 : 0.0;
  }

  throw std::runtime_error("Value is not double-compatible (REF?)");
}

bool Value::asBool() const {
  if (_type == ValueType::BOOL) {
    return std::get<bool>(_data);
  }
  if (_type == ValueType::INT) {
    return std::get<int64_t>(_data) != 0;
  }
  if (_type == ValueType::DOUBLE) {
    return std::fabs(std::get<double>(_data)) > 1e-9;
  }

  throw std::runtime_error("Value is not bool-compatible (REF?)");
}

HeapValue *Value::asHeapRef() const {
  if (_type == ValueType::REF) {
    return std::get<HeapValue *>(_data);
  }

  throw std::runtime_error("Value is not heap reference");
}

std::variant<int64_t, double, bool, HeapValue *> Value::getValue() const {
  return _data;
}

ValueType Value::getType() const {
  return _type;
}

bool Value::isHeapRef() const {
  return _type == ValueType::REF;
}

Value operator+(const Value &lhs, const Value &rhs) {
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

Value operator-(const Value &lhs, const Value &rhs) {
  if (lhs._type == ValueType::REF && rhs._type == ValueType::REF) {
    throw std::runtime_error("Cannot subtract references (REF)");
  }

  const bool useDouble = (lhs._type == ValueType::DOUBLE || rhs._type == ValueType::DOUBLE);

  if (useDouble) {
    const double diff = lhs.asDouble() - rhs.asDouble();
    return Value(diff);
  }

  const int64_t diff = lhs.asInt() - rhs.asInt();
  return Value(diff);
}

Value operator*(const Value &lhs, const Value &rhs) {
  if (lhs._type == ValueType::REF || rhs._type == ValueType::REF) {
    throw std::runtime_error("Cannot multiply references (REF)");
  }
  bool useDouble = (lhs._type == ValueType::DOUBLE || rhs._type == ValueType::DOUBLE);

  if (useDouble) {
    const double prod = lhs.asDouble() * rhs.asDouble();
    return Value(prod);
  }

  const int64_t prod = lhs.asInt() * rhs.asInt();
  return Value(prod);
}

Value operator/(const Value &lhs, const Value &rhs) {
  if (lhs._type == ValueType::REF || rhs._type == ValueType::REF) {
    throw std::runtime_error("Cannot divide references (REF)");
  }
  const bool useDouble = (lhs._type == ValueType::DOUBLE || rhs._type == ValueType::DOUBLE);

  if (rhs.asDouble() == 0.0) {
    throw std::runtime_error("Division by zero");
  }

  if (useDouble) {
    const double quotient = lhs.asDouble() / rhs.asDouble();
    return Value(quotient);
  }

  const int64_t b = rhs.asInt();
  if (b == 0) {
    throw std::runtime_error("Division by zero (int)");
  }
  const int64_t a = lhs.asInt();
  return Value(a / b);
}

Value operator%(const Value &lhs, const Value &rhs) {
  if (lhs._type == ValueType::REF || rhs._type == ValueType::REF) {
    throw std::runtime_error("Cannot perform modulo with references (REF)");
  }
  const bool useDouble = (lhs._type == ValueType::DOUBLE || rhs._type == ValueType::DOUBLE);

  if (rhs.asDouble() == 0.0) {
    throw std::runtime_error("Division by zero");
  }

  if (useDouble) {
    throw std::runtime_error("Modulo operation only supported for integers");
  }

  const int64_t b = rhs.asInt();
  if (b == 0) {
    throw std::runtime_error("Division by zero (int)");
  }
  const int64_t a = lhs.asInt();
  return Value(a % b);
}

bool operator==(const Value &lhs, const Value &rhs) {
  if (lhs._type == ValueType::REF && rhs._type == ValueType::REF) {
    return lhs.asHeapRef() == rhs.asHeapRef();
  }
  if (lhs._type == ValueType::REF || rhs._type == ValueType::REF) {
    return false;
  }

  const bool useDouble = (lhs._type == ValueType::DOUBLE || rhs._type == ValueType::DOUBLE);
  if (useDouble) {
    return (lhs.asDouble() == rhs.asDouble());
  }

  return (lhs.asInt() == rhs.asInt());
}

bool operator!=(const Value &lhs, const Value &rhs) {
  return !(lhs == rhs);
}

bool operator<(const Value &lhs, const Value &rhs) {
  if (lhs._type == ValueType::REF && rhs._type == ValueType::REF) {
    return lhs.asHeapRef() < rhs.asHeapRef();
  }
  if (lhs._type == ValueType::REF || rhs._type == ValueType::REF) {
    return false;
  }

  const bool useDouble = (lhs._type == ValueType::DOUBLE || rhs._type == ValueType::DOUBLE);
  if (useDouble) {
    return (lhs.asDouble() < rhs.asDouble());
  }

  return (lhs.asInt() < rhs.asInt());
}

bool operator<=(const Value &lhs, const Value &rhs) {
  return (lhs < rhs || lhs == rhs);
}

bool operator>(const Value &lhs, const Value &rhs) {
  return !(lhs <= rhs);
}

bool operator>=(const Value &lhs, const Value &rhs) {
  return !(lhs < rhs);
}

Value operator!(const Value &val) {
  return Value(!val.asBool());
}

Value operator-(const Value &val) {
  if (val.getType() == ValueType::INT) {
    return Value(-val.asInt());
  }
  return Value(-val.asDouble());
}

bool operator&&(const Value &lhs, const Value &rhs) {
  return lhs.asBool() && rhs.asBool();
}

bool operator||(const Value &lhs, const Value &rhs) {
  return lhs.asBool() || rhs.asBool();
}
