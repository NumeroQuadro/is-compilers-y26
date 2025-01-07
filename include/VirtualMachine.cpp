#include "VirtualMachine.h"
#include <iostream>
#include <stdexcept>
#include <functional>

#include "FunctionInfo.h"
#include "GarbageCollector.h"


VirtualMachine::VirtualMachine(): gc(nullptr) {
  scope_ = new Scope(nullptr);

  functionTable["__pushBack"] = FunctionInfo{
    "__pushBack",
    {{"array", ValueType::REF}, {"value"}},
    -1ll
  };

  functionTable["__popBack"] = FunctionInfo{
    "__popBack",
    {{"array", ValueType::REF}},
    -1ll
  };

  functionTable["__size"] = FunctionInfo{
    "__size",
    {{"array", ValueType::REF}},
    -1ll
  };
}

VirtualMachine::VirtualMachine(const std::vector<Instruction> &code,
                               const std::unordered_map<std::string, FunctionInfo> &functions,
                               GarbageCollector *garbageCollector)
  : gc(garbageCollector), functionTable(functions), code(code) {
  scope_ = new Scope(nullptr);

  functionTable["__pushBack"] = FunctionInfo{
    "__pushBack",
    {{"array", ValueType::REF}, {"value"}},
    -1ll
  };

  functionTable["__popBack"] = FunctionInfo{
    "__popBack",
    {{"array", ValueType::REF}},
    -1ll
  };

  functionTable["__size"] = FunctionInfo{
    "__size",
    {{"array", ValueType::REF}},
    -1ll
  };
  waitFile = false;
}

void VirtualMachine::builtInPushBack() {
  if (stack.size() < 2) {
    throw std::runtime_error("pushBack requires 2 arguments: array and value");
  }

  const Value value = pop();
  const Value arrayValue = pop();

  if (arrayValue.getType() != ValueType::REF) {
    throw std::runtime_error("popBack requires an array reference");
  }

  auto *array = dynamic_cast<ArrayValue *>(arrayValue.asHeapRef());
  if (!array) {
    throw std::runtime_error("pushBack requires an array reference");
  }
  array->elements.push_back(value);
  ++ip;
}

void VirtualMachine::builtInPopBack() {
  if (stack.empty()) {
    throw std::runtime_error("popBack requires 1 argument: array");
  }

  const Value arrayValue = pop();

  if (arrayValue.getType() != ValueType::REF) {
    throw std::runtime_error("popBack requires an array reference");
  }
  auto *array = dynamic_cast<ArrayValue *>(arrayValue.asHeapRef());
  if (!array) {
    throw std::runtime_error("popBack requires an array reference");
  }
  if (array->elements.empty()) {
    throw std::runtime_error("Cannot popBack from an empty array");
  }

  const Value popped = array->elements.back();
  array->elements.pop_back();
  push(popped);

  ++ip;
}

void VirtualMachine::builtInSize() {
  if (stack.empty()) {
    throw std::runtime_error("size requires 1 argument: array");
  }

  const Value arrayValue = pop();

  if (arrayValue.getType() != ValueType::REF) {
    throw std::runtime_error("size requires an array reference");
  }
  const auto *array = dynamic_cast<ArrayValue *>(arrayValue.asHeapRef());
  if (!array) {
    throw std::runtime_error("size requires an array reference");
  }

  const auto size = static_cast<int64_t>(array->elements.size());
  push(Value(size));

  ++ip;
}

VirtualMachine::VirtualMachine(const std::vector<Instruction> &code,
                               const std::unordered_map<std::string, FunctionInfo> &functions,
                               GarbageCollector *garbageCollector,
                               const int64_t startPos)
  : VirtualMachine(code, functions, garbageCollector) {
  ip = startPos;
  start_address = ip;
}

Value VirtualMachine::pop() {
  if (stack.empty()) throw std::runtime_error("Stack underflow");
  const Value v = stack.top();
  stack.pop();
  if (v.isHeapRef() && !heapRefs.empty()) {
    heapRefs.pop_back();
  }
  return v;
}

Value VirtualMachine::top() const {
  if (stack.empty()) throw std::runtime_error("Stack underflow");
  return stack.top();
}

void print(const Value &value) {
  switch (value.getType()) {
    case ValueType::INT: {
      std::cout << std::to_string(value.asInt());
      break;
    }
    case ValueType::BOOL: {
      std::cout << (value.asBool() ? "true" : "false");
      break;
    }
    case ValueType::DOUBLE: {
      std::cout << value.asDouble();
      break;
    }
    case ValueType::REF: {
      if (const auto str = dynamic_cast<StringValue *>(value.asHeapRef())) {
        std::cout << '\"' + str->value << '\"';
        break;
      }
      if (const auto arr = dynamic_cast<ArrayValue *>(value.asHeapRef())) {
        std::cout << "[";
        for (size_t i = 0; i < arr->elements.size(); ++i) {
          print(arr->elements[i]);
          if (i != arr->elements.size() - 1) {
            std::cout << ", ";
          }
        }
        std::cout << "]";
      }
      break;
    }
    default: {
      throw std::runtime_error("Value has not supported type");
    }
  }
  std::cout << "\n";
}

void VirtualMachine::push(const Value &v) {
  stack.emplace(v);
  if (v.isHeapRef()) {
    heapRefs.emplace_back(v.asHeapRef());
  }
}

HeapValue *VirtualMachine::allocHeap(std::unique_ptr<HeapValue> hv) const {
  return gc->allocObject(std::move(hv));
}

Value VirtualMachine::loadVar(const std::string &name) const {
  Value out;
  if (!scope_->findVar(name, out)) {
    throw std::runtime_error("Undefined variable: " + name);
  }
  return out;
}

void VirtualMachine::storeVar(const std::string &name, const Value &val) const {
  if (!scope_->setVar(name, val)) {
    scope_->createVar(name, val);
  }
}

void VirtualMachine::createVar(const std::string &name, const Value &val) const {
  scope_->createVar(name, val);
}

void VirtualMachine::enterScope() {
  scope_ = new Scope(scope_);
}

void VirtualMachine::exitScope() {
  scope_->isClosed = true;

  gc->collectGarbage(heapRefs, scope_);

  const Scope *old = scope_;
  scope_ = old->previous_;
}

bool VirtualMachine::hasPrint(const std::string &funcName) const {
  const auto func = functionTable.find(funcName);
  if (func == functionTable.end()) {
    throw std::runtime_error("Function not found: " + funcName);
  }

  const auto &[name, params, address] = func->second;

  size_t i = address;
  while (i < code.size() && code[i].op != InstructionType::RET) {
    if (code[i].op == InstructionType::PRINT) {
      return true;
    }
    ++i;
  }
  return false;
}

void VirtualMachine::doCall(const std::string &funcName) {
  if (funcName == "__pushBack") {
    builtInPushBack();
    return;
  }

  if (funcName == "__popBack") {
    builtInPopBack();
    return;
  }

  if (funcName == "__size") {
    builtInSize();
    return;
  }

  if (isOptimized && optimized_functions.find(funcName) == optimized_functions.end()) {
    optimizeFunction(funcName);
  }

  const auto funcInfo = functionTable.find(funcName);
  if (funcInfo == functionTable.end()) {
    throw std::runtime_error("Function not found: " + code[ip].toStr());
  }

  const auto &[name, params, address] = funcInfo->second;
  const auto newScope = new Scope(nullptr);

  std::vector<std::pair<std::string, Value> > paramValues;
  bool hasRefParam = false;
  for (int64_t i = params.size() - 1; i >= 0; --i) {
    Value argVal = pop();
    if (argVal.getType() != params[i].type) {
      throw std::runtime_error("Parameter type mismatch");
    }

    hasRefParam = hasRefParam || argVal.isHeapRef();
    paramValues.emplace_back(params[i].name, argVal);
  }

  auto frame = CallFrame{
    ip + 1,
    scope_,
    true
  };

  if (isOptimized && !hasPrint(funcName) && !hasRefParam) {
    if (const auto &funcCache = functionsCallCache.find(funcName); funcCache != functionsCallCache.end()) {
      frame.funcName = funcName;
      callStack.push(frame);
      stack.emplace(funcCache->second);
      doRet();
      return;
    }
  }

  for (const auto &[name, value]: paramValues) {
    newScope->createVar(name, value);
  }

  callStack.push(frame);

  scope_ = newScope;
  ip = address;
}

void VirtualMachine::doRet() {
  Value retVal;
  if (!stack.empty()) {
    retVal = pop();
  }

  if (callStack.empty()) {
    ip = static_cast<int64_t>(code.size());
    return;
  }

  const auto [returnIp, prevScope, hasReturnValue, funcName] = callStack.top();
  if (isOptimized && !funcName.empty()) {
    functionsCallCache[funcName] = retVal;
  }
  callStack.pop();

  delete scope_;
  scope_ = prevScope;

  if (hasReturnValue) {
    push(retVal);
  }

  ip = returnIp;
}

VirtualMachine::~VirtualMachine() {
  while (scope_) {
    const auto p = scope_->previous_;
    delete scope_;
    scope_ = p;
  }
}

void VirtualMachine::run() {
  if (waitFile) {
    std::cerr << "There was no instructions loaded! Use for example fromFile to do it!\n";
    return;
  }

  while (ip >= 0 && ip < code.size()) {
    ++runnedOperationsCount;
    switch (const Instruction &inst = code[ip]; inst.op) {
      case InstructionType::JMP:
        ip = inst.intOperand;
        break;
      case InstructionType::PUSH_INT: {
        push(Value(inst.intOperand));
        ++ip;
        break;
      }
      case InstructionType::PUSH_DOUBLE: {
        push(Value(inst.doubleOperand));
        ++ip;
        break;
      }
      case InstructionType::PUSH_BOOL: {
        push(Value(inst.boolOperand));
        ++ip;
        break;
      }
      case InstructionType::PUSH_STRING: {
        auto hv = std::make_unique<StringValue>(inst.strOperand);
        HeapValue *ref = allocHeap(std::move(hv));
        push(Value(ref));
        ++ip;
        break;
      }
      case InstructionType::PUSH_VAR: {
        Value v = loadVar(inst.strOperand);
        push(v);
        ++ip;
        break;
      }
      case InstructionType::STORE_VAR: {
        Value v = pop();
        storeVar(inst.strOperand, v);
        ++ip;
        break;
      }
      case InstructionType::ADD: {
        const Value a = pop();
        const Value b = pop();
        push(Value(a + b));
        ++ip;
        break;
      }
      case InstructionType::SUB: {
        const Value b = pop();
        const Value a = pop();
        push(Value(a - b));
        ++ip;
        break;
      }
      case InstructionType::MUL: {
        const Value a = pop();
        const Value b = pop();
        push(Value(a * b));
        ++ip;
        break;
      }
      case InstructionType::DIV: {
        Value b = pop();
        Value a = pop();
        push(Value(a / b));
        ++ip;
        break;
      }
      case InstructionType::DIV_REM: {
        Value b = pop();
        Value a = pop();
        push(Value(a % b));
        ++ip;
        break;
      }
      case InstructionType::AND: {
        Value b = pop();
        Value a = pop();
        push(Value(a.asBool() && b.asBool()));
        ++ip;
        break;
      }
      case InstructionType::OR: {
        Value b = pop();
        Value a = pop();
        push(Value(a.asBool() || b.asBool()));
        ++ip;
        break;
      }
      case InstructionType::EQ: {
        const Value b = pop();
        const Value a = pop();
        const bool r = (a == b);
        push(Value(r));
        ++ip;
        break;
      }
      case InstructionType::NEQ: {
        const Value b = pop();
        const Value a = pop();
        const bool r = (a != b);
        push(Value(r));
        ++ip;
        break;
      }
      case InstructionType::LT: {
        const Value b = pop();
        const Value a = pop();
        const bool r = (a < b);
        push(Value(r));
        ++ip;
        break;
      }
      case InstructionType::LE: {
        const Value b = pop();
        const Value a = pop();
        const bool r = (a <= b);
        push(Value(r));
        ++ip;
        break;
      }
      case InstructionType::GT: {
        const Value b = pop();
        const Value a = pop();
        const bool r = (a > b);
        push(Value(r));
        ++ip;
        break;
      }
      case InstructionType::GE: {
        const Value b = pop();
        const Value a = pop();
        const bool r = (a >= b);
        push(Value(r));
        ++ip;
        break;
      }
      case InstructionType::NOT: {
        Value v = pop();
        bool val = v.asBool();
        push(Value(!val));
        ++ip;
        break;
      }
      case InstructionType::NEG: {
        Value v = pop();
        int64_t vi = v.asInt();
        push(Value(-vi));
        ++ip;
        break;
      }
      case InstructionType::JMZ: {
        Value v = pop();
        if (const bool val = v.asBool(); !val) {
          ip = inst.intOperand;
        } else {
          ++ip;
        }
        break;
      }
      case InstructionType::PRINT: {
        if (!stack.empty()) {
          Value v = pop();
          print(v);
        } else {
          std::cout << "\n";
        }
        ++ip;
        break;
      }
      case InstructionType::NEW_ARRAY: {
        int64_t size;
        if (inst.strOperand == "__stack") {
          if (stack.empty()) {
            throw std::runtime_error("NEW_ARRAY: stack underflow (need size on stack)");
          }
          Value sizeVal = pop();
          size = sizeVal.asInt();
        } else {
          size = inst.intOperand;
        }

        if (size < 0) {
          throw std::runtime_error("NEW_ARRAY: negative size is not allowed");
        }

        auto arrPtr = std::make_unique<ArrayValue>(size);
        HeapValue *ref = allocHeap(std::move(arrPtr));

        push(Value(ref));

        ++ip;
        break;

        // auto arr = std::make_unique<ArrayValue>(inst.intOperand);
        // HeapValue *ref = allocHeap(std::move(arr));
        // push(Value(ref));
        // ip++;
        // break;
      }
      case InstructionType::GET_ELEMENT: {
        Value idxVal = pop();
        Value arrVal = pop();
        int64_t idx = idxVal.asInt();
        auto *arr = dynamic_cast<ArrayValue *>(arrVal.asHeapRef());
        if (!arr) throw std::runtime_error("Not an array");
        if (idx < 0 || idx >= arr->elements.size()) throw std::runtime_error("Array index out of bounds");
        push(arr->getValue(idx));
        ++ip;
        break;
      }
      case InstructionType::SET_ELEMENT: {
        Value val = pop();
        Value idxVal2 = pop();
        Value arrVal2 = pop();
        int64_t idx2 = idxVal2.asInt();
        auto *arr2 = dynamic_cast<ArrayValue *>(arrVal2.asHeapRef());
        if (!arr2) throw std::runtime_error("Not an array");
        if (idx2 < 0 || idx2 >= arr2->elements.size())
          throw std::runtime_error(
            "Array index out of bounds" + code[ip].toStr());
        arr2->setValue(idx2, val);
        ++ip;
        break;
      }
      case InstructionType::ENTER_SCOPE:
        enterScope();
        ++ip;
        break;
      case InstructionType::EXIT_SCOPE:
        exitScope();
        ++ip;
        break;
      case InstructionType::DUP_TOP: {
        Value v = top();
        push(v);
        ++ip;
        break;
      }
      case InstructionType::CALL: {
        doCall(inst.strOperand);
        break;
      }
      case InstructionType::RET: {
        doRet();
        break;
      }
      case InstructionType::HALT:
        return;
      default:
        throw std::runtime_error("Unknown instruction at ip " + std::to_string(ip));
    }
  }
}

Scope *VirtualMachine::getCurrentScope() const {
  return scope_;
}

std::stack<Value> &VirtualMachine::getStackRef() {
  return stack;
}

void VirtualMachine::fromFile(const std::string &path) {
  std::ifstream inFile(path);
  if (!inFile.is_open()) {
    throw std::runtime_error("Failed to open file: " + path);
  }

  std::string line;

  if (!std::getline(inFile, line)) {
    throw std::runtime_error("Failed to read startPos");
  }
  ip = std::stoull(line);

  if (!std::getline(inFile, line)) {
    throw std::runtime_error("Failed to read function count");
  }
  size_t functionCount = std::stoull(line);

  for (size_t i = 0; i < functionCount; ++i) {
    if (!std::getline(inFile, line)) {
      throw std::runtime_error("Failed to read function table entry");
    }

    std::istringstream iss(line);
    FunctionInfo funcInfo;
    iss >> funcInfo.name >> funcInfo.address;


    int tmp = 0;
    std::string input;
    std::string param_name;
    ValueType type;
    while (iss >> input) {
      if (tmp % 2 == 0) {
        param_name = input;
      } else {
        if (input == "INT") {
          type = ValueType::INT;
        } else if (input == "BOOL") {
          type = ValueType::BOOL;
        } else if (input == "DOUBLE") {
          type = ValueType::DOUBLE;
        } else if (input == "REF") {
          type = ValueType::REF;
        } else {
          throw std::runtime_error("Unknown type!");
        }
        funcInfo.params.push_back({param_name, type});
      }
      tmp++;
    }

    functionTable[funcInfo.name] = funcInfo;
  }

  if (!std::getline(inFile, line)) {
    throw std::runtime_error("Failed to read instruction count");
  }
  size_t instructionCount = std::stoull(line);

  code.clear();
  for (size_t i = 0; i < instructionCount; ++i) {
    if (!std::getline(inFile, line)) {
      throw std::runtime_error("Failed to read instruction");
    }
    code.push_back(Instruction::fromStr(line));
  }

  start_address = ip;
  waitFile = false;
}

std::vector<Instruction> VirtualMachine::getInstructions() const {
  return code;
}

void VirtualMachine::optimize(bool optimizeOn = true) {
  isOptimized = optimizeOn;
}

size_t VirtualMachine::getHeapSize() const {
  return heapRefs.size();
}

void VirtualMachine::optimizeFunction(const std::string &function_name) {
  auto iter = functionTable.find(function_name);
  if (iter == functionTable.end()) {
    throw std::runtime_error("Function not found: " + code[ip].toStr());
  }

  int64_t address = iter->second.address;
  int64_t next_function_address = start_address;
  for (auto &p: functionTable) {
    if (p.second.address > address) {
        next_function_address = std::min(next_function_address, p.second.address);
    }
  }

    std::unordered_set<std::string> usedVariables;
    std::unordered_map<std::string, int64_t> save_usages;
    for (int64_t i = next_function_address - 1; i >= address; i--) {
        if (code[i].op == InstructionType::STORE_VAR) {
            save_usages[code[i].strOperand]++;
        }
    }
    for (auto& p : save_usages) {
        if (p.second > 1) {
            usedVariables.insert(p.first);
        }
    }

  for (int64_t i = next_function_address - 1; i >= address; i--) {
    if (code[i].op == InstructionType::RET || code[i].op == InstructionType::PUSH_VAR) {
      usedVariables.insert(code[i].strOperand);
    } else if (code[i].op == InstructionType::STORE_VAR) {
      if (usedVariables.find(code[i].strOperand) == usedVariables.end()) {
        int64_t k = 1;
        --i;
        while ((code[i].op == InstructionType::ADD
               || code[i].op == InstructionType::MUL
               || code[i].op == InstructionType::SUB
               || code[i].op == InstructionType::DIV
               || code[i].op == InstructionType::DIV_REM
               || code[i].op == InstructionType::PUSH_INT
               || code[i].op == InstructionType::PUSH_BOOL
               || code[i].op == InstructionType::PUSH_DOUBLE
               || code[i].op == InstructionType::PUSH_STRING
               || code[i].op == InstructionType::PUSH_VAR
               || code[i].op == InstructionType::AND
               || code[i].op == InstructionType::OR
               || code[i].op == InstructionType::EQ
               || code[i].op == InstructionType::NEQ
                || code[i].op == InstructionType::LT
                || code[i].op == InstructionType::LE
                || code[i].op == InstructionType::GT
                || code[i].op == InstructionType::GE
                || code[i].op == InstructionType::NOT
                || code[i].op == InstructionType::NEG) && i >= 0) {
          --i;
          k++;
        }
        ++i;
        code[i] = Instruction(InstructionType::JMP, k + i);
        while (code[i + k].op == InstructionType::JMP) {
          k = code[i + k].intOperand - i;
          code[i] = Instruction(InstructionType::JMP, k + i);
        }
        continue;
      }
    }
  }

//  std::cout << "\n After Dead code ellumination:\n";
//    for (int64_t i = address; i < next_function_address; i++) {
//        std::cout << i << " " << code[i].toStr() << '\n';
//    }
    foldConstants(address, next_function_address);

  optimized_functions.insert(function_name);
}

struct StackEntry {
  Value value;
  bool isConstant;
  std::string varName;
};

Value calcArithmetic(InstructionType op, Value a, Value b) {
  switch (op) {
    case InstructionType::ADD: return a + b;
    case InstructionType::SUB: return a - b;
    case InstructionType::MUL: return a * b;
    case InstructionType::DIV: return a / b;
    case InstructionType::DIV_REM: return a % b;
    default: throw std::runtime_error("Unsupported arithmetic operation!");
  }
}

bool calcComparison(InstructionType op, Value a, Value b) {
  switch (op) {
    case InstructionType::EQ: return a == b;
    case InstructionType::NEQ: return a != b;
    case InstructionType::LT: return a < b;
    case InstructionType::LE: return a <= b;
    case InstructionType::GT: return a > b;
    case InstructionType::GE: return a >= b;
    case InstructionType::AND: return a && b;
    case InstructionType::OR: return a || b;
    default: throw std::runtime_error("Unsupported comparison operation!");
  }
}

Value calcUnary(InstructionType op, Value a) {
  switch (op) {
    case InstructionType::NEG: return -a;
    case InstructionType::NOT: return !a;
    default: throw std::runtime_error("Unsupported unary operation!");
  }
}

std::vector<Instruction> VirtualMachine::foldConstants(int64_t start, int64_t finish) {
  std::vector<Instruction> optimizedBody;
  std::vector<StackEntry> stack;

  for (int64_t i = start; i < finish; i++) {
    Instruction instr = code[i];
    switch (instr.op) {
      case InstructionType::JMP:
        optimizedBody.push_back(code[i]);
        if (instr.intOperand > i) {
          /*for (int64_t j = i + 1; j < instr.intOperand; j++) {
              optimizedBody.push_back(code[j]);
          }*/
          i = instr.intOperand - 1;
        }
        break;
      case InstructionType::PUSH_INT:
        stack.push_back({Value(instr.intOperand), true, ""});
        optimizedBody.push_back(instr);
        break;
      case InstructionType::PUSH_STRING:
        stack.push_back({Value(new StringValue(instr.strOperand)), true, ""});
        optimizedBody.push_back(instr);
        break;
      case InstructionType::PUSH_BOOL:
        stack.push_back({Value(instr.boolOperand), true, ""});
        optimizedBody.push_back(instr);
        break;
      case InstructionType::PUSH_VAR:
        stack.push_back({Value(), false, instr.strOperand});
        optimizedBody.push_back(instr);
        break;
      case InstructionType::ADD:
      case InstructionType::SUB:
      case InstructionType::MUL:
      case InstructionType::DIV:
      case InstructionType::DIV_REM: {
        if (stack.size() >= 2) {
          auto b = stack.back();
          stack.pop_back();
          auto a = stack.back();
          stack.pop_back();

          if (a.isConstant && b.isConstant) {
            Value result = calcArithmetic(instr.op, a.value, b.value);
            stack.push_back({result, true, ""});
            optimizedBody.pop_back();
            optimizedBody.pop_back();
            if (result.getType() == ValueType::INT)
              optimizedBody.emplace_back(Instruction(InstructionType::PUSH_INT, result.asInt()));
            if (result.getType() == ValueType::BOOL)
              optimizedBody.emplace_back(Instruction(InstructionType::PUSH_BOOL, result.asBool()));
            if (result.getType() == ValueType::DOUBLE)
              optimizedBody.emplace_back(Instruction(InstructionType::PUSH_INT, result.asDouble()));
          } else {
            stack.push_back({Value(), false, ""});
            optimizedBody.push_back(instr);
          }
        }
        break;
      }
      case InstructionType::EQ:
      case InstructionType::NEQ:
      case InstructionType::LT:
      case InstructionType::LE:
      case InstructionType::GT:
      case InstructionType::GE:
      case InstructionType::AND:
      case InstructionType::OR: {
        if (stack.size() >= 2) {
          auto b = stack.back();
          stack.pop_back();
          auto a = stack.back();
          stack.pop_back();

          if (a.isConstant && b.isConstant) {
            bool result = calcComparison(instr.op, a.value, b.value);
            stack.push_back({Value(result), true, ""});
            optimizedBody.pop_back();
            optimizedBody.pop_back();
            optimizedBody.emplace_back(Instruction(InstructionType::PUSH_BOOL, result));
          } else {
            stack.push_back({Value(), false, ""});
            optimizedBody.push_back(instr);
          }
        }
        break;
      }
      case InstructionType::NEG:
      case InstructionType::NOT: {
        if (!stack.empty()) {
          auto a = stack.back();
          stack.pop_back();

          if (a.isConstant) {
            Value result = calcUnary(instr.op, a.value);
            stack.push_back({result, true, ""});
            optimizedBody.pop_back();
            if (result.getType() == ValueType::INT)
              optimizedBody.emplace_back(Instruction(InstructionType::PUSH_INT, result.asInt()));
            if (result.getType() == ValueType::BOOL)
              optimizedBody.emplace_back(Instruction(InstructionType::PUSH_BOOL, result.asBool()));
            if (result.getType() == ValueType::DOUBLE)
              optimizedBody.emplace_back(Instruction(InstructionType::PUSH_INT, result.asDouble()));
          } else {
            stack.push_back({Value(), false, ""});
            optimizedBody.push_back(instr);
          }
        }
        break;
      }
      case InstructionType::STORE_VAR:
        if (!stack.empty()) {
          stack.pop_back();
        }
        optimizedBody.push_back(instr);
        break;
      default:
        optimizedBody.push_back(instr);
        break;
    }
  }
  //
  //    std::cout << "\n Optimized commands:\n";
  //    for (int i = 0; i < optimizedBody.size(); i++) {
  //        std::cout << i << " " << optimizedBody[i].toStr() << '\n';
  //    }
  //    std::cout << " Optimized commands end\n";

  int64_t j = start;
  for (int64_t i = 0; i < optimizedBody.size();) {
    if (optimizedBody[i] == code[j] && !(i + 1 < optimizedBody.size()
                                         && (optimizedBody[i + 1].op == InstructionType::STORE_VAR
                                             || optimizedBody[i + 1].op == InstructionType::JMZ
                                             || optimizedBody[i + 1].op == InstructionType::PRINT
                                             || optimizedBody[i + 1].op == InstructionType::SET_ELEMENT
                                             || optimizedBody[i + 1].op == InstructionType::RET
                                             || optimizedBody[i + 1].op == InstructionType::NEW_ARRAY
                                             || optimizedBody[i + 1].op == InstructionType::CALL))) {
      i++;
      if (code[j].op == InstructionType::JMP && code[j].intOperand > j) {
        j = code[j].intOperand;
        continue;
      }
      j++;
    } else {
      if (code[j] == optimizedBody[i] && code[j + 1] == optimizedBody[i + 1]) {
        j += 2;
        i += 2;
        continue;
      }
      while (optimizedBody[i].op != InstructionType::STORE_VAR
             && optimizedBody[i].op != InstructionType::JMZ
             && optimizedBody[i].op != InstructionType::PRINT
             && optimizedBody[i].op != InstructionType::SET_ELEMENT
             && optimizedBody[i].op != InstructionType::RET
             && optimizedBody[i].op != InstructionType::NEW_ARRAY
             && optimizedBody[i].op != InstructionType::CALL) {
        code[j] = optimizedBody[i];
        j++;
        i++;
      }

      // if (optimizedBody[i].op == InstructionType::STORE_VAR) {
      code[j] = optimizedBody[i];
      i++;
      j++;
      int64_t place_to_jump = j;
      while (code[j].op != InstructionType::STORE_VAR
             && code[j].op != InstructionType::JMZ
             && code[j].op != InstructionType::PRINT
             && code[j].op != InstructionType::SET_ELEMENT
             && code[j].op != InstructionType::RET
             && code[j].op != InstructionType::NEW_ARRAY
             && code[j].op != InstructionType::CALL) {
        j++;
      }
      j++;
      code[place_to_jump] = Instruction(InstructionType::JMP, (int64_t) j);
    };
  }

  //    std::cout << "\nAFTER OPTIMIZATIONS\n";
  //    for (int64_t i = start; i < finish; i++) {
  //        std::cout << i << " " << code[i].toStr() << '\n';
  //    }

  return optimizedBody;
}

size_t VirtualMachine::getRunnedOperationsCount() {
    return runnedOperationsCount;
}

