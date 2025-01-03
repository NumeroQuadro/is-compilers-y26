#include "VirtualMachine.h"
#include <iostream>
#include <stdexcept>
#include <functional>

#include "FunctionInfo.h"


VirtualMachine::VirtualMachine() {
    scope_ = new Scope(nullptr);

    functionTable["__pushBack"] = FunctionInfo{
            "__pushBack",
            {"array", "value"},
            -1ll
    };

    functionTable["__popBack"] = FunctionInfo{
            "__popBack",
            {"array"},
            -1ll
    };

    functionTable["__size"] = FunctionInfo{
            "__size",
            {"array"},
            -1ll
    };
}

VirtualMachine::VirtualMachine(const std::vector<Instruction> &code,
                               const std::unordered_map<std::string, FunctionInfo> &functions)
        : functionTable(functions), code(code) {
    scope_ = new Scope(nullptr);

    functionTable["__pushBack"] = FunctionInfo{
            "__pushBack",
            {"array", "value"},
            -1ll
    };

    functionTable["__popBack"] = FunctionInfo{
            "__popBack",
            {"array"},
            -1ll
    };

    functionTable["__size"] = FunctionInfo{
            "__size",
            {"array"},
            -1ll
    };
    waitFile = false;
    start_adress = ip;
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
                               const std::unordered_map<std::string, FunctionInfo> &functions, const int64_t startPos)
        : VirtualMachine(code, functions) {
    ip = startPos;
    start_adress = startPos;
}

Value VirtualMachine::pop() {
    if (stack.empty()) throw std::runtime_error("Stack underflow");
    const Value v = stack.top();
    stack.pop();
    return v;
}

Value VirtualMachine::top() {
    if (stack.empty()) throw std::runtime_error("Stack underflow");
    return stack.top();
}

void VirtualMachine::push(const Value &v) {
    stack.push(v);
}

template<typename Op>
void VirtualMachine::binaryOp(Op op) {
    Value b = pop();
    Value a = pop();
    if (a.getType() != b.getType())
        throw std::runtime_error("Different types of a and b exception in binaryOp!");
    size_t ai = a.asInt();
    size_t bi = b.asInt();
    push(Value(op(ai, bi)));
    ip++;
}

template<typename Op>
void VirtualMachine::cmpOp(Op op) {
    Value b = pop();
    Value a = pop();
    if (a.getType() != b.getType())
        throw std::runtime_error("Different types of a and b exception in cmpOp!");
    size_t ai, bi;
    if (a.getType() == ValueType::BOOL) {
        ai = a.asBool();
        bi = b.asBool();
    } else {
        ai = a.asInt();
        bi = b.asInt();
    }
    bool r = op(ai, bi);
    push(Value(r));
    ip++;
}

HeapValue *VirtualMachine::allocHeap(std::unique_ptr<HeapValue> hv) {
    HeapValue *ptr = hv.get();
    heap.push_back(std::move(hv));
    return ptr;
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
    Scope *old = scope_;
    scope_ = scope_->previous_;
    delete old;
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

    auto it = functionTable.find(funcName);
    if (it == functionTable.end()) {
        throw std::runtime_error("Function not found: " + code[ip].toStr());
    }

    const auto &[name, paramNames, address] = it->second;
    const auto newScope = new Scope(nullptr);

    for (int64_t i = paramNames.size() - 1; i >= 0; i--) {
        Value argVal = pop();
        newScope->createVar(paramNames[i], argVal);
    }

    const auto frame = CallFrame{
            ip + 1,
            scope_,
            true
    };

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

    const auto [returnIp, prevScope, hasReturnValue] = callStack.top();
    callStack.pop();

    delete scope_;
    scope_ = prevScope;

    if (hasReturnValue) {
        push(retVal);
    }

    ip = returnIp;
}

void VirtualMachine::run() {
    if (waitFile) {
        std::cerr << "There was no instructions loaded! Use for example fromFile to do it!\n";
        return;
    }

    while (ip >= 0 && ip < code.size()) {
        switch (const Instruction &inst = code[ip]; inst.op) {
            case InstructionType::JMP:
                ip = inst.intOperand;
                break;
            case InstructionType::PUSH_INT:
                push(Value(inst.intOperand));
                ip++;
                break;
            case InstructionType::PUSH_BOOL:
                push(Value(inst.boolOperand));
                ip++;
                break;
            case InstructionType::PUSH_STRING: {
                auto hv = std::make_unique<StringValue>(inst.strOperand);
                HeapValue *ref = allocHeap(std::move(hv));
                push(Value(ref));
                ip++;
                break;
            }
            case InstructionType::PUSH_VAR: {
                Value v = loadVar(inst.strOperand);
                push(v);
                ip++;
                break;
            }
            case InstructionType::STORE_VAR: {
                Value v = pop();
                storeVar(inst.strOperand, v);
                ip++;
                break;
            }
            case InstructionType::ADD:
                binaryOp(std::plus<int64_t>());
                break;
            case InstructionType::SUB:
                binaryOp(std::minus<int64_t>());
                break;
            case InstructionType::MUL:
                binaryOp(std::multiplies<int64_t>());
                break;
            case InstructionType::DIV: {
                Value b = pop();
                Value a = pop();
                int64_t bi = b.asInt();
                if (bi == 0) {
                    throw std::runtime_error("Division by zero");
                }
                int64_t ai = a.asInt();
                push(Value(ai / bi));
                ip++;
                break;
            }
            case InstructionType::EQ:
                cmpOp(std::equal_to<size_t>());
                break;
            case InstructionType::NEQ:
                cmpOp(std::not_equal_to<size_t>());
                break;
            case InstructionType::LT:
                cmpOp(std::less<size_t>());
                break;
            case InstructionType::LE:
                cmpOp(std::less_equal<size_t>());
                break;
            case InstructionType::GT:
                cmpOp(std::greater<size_t>());
                break;
            case InstructionType::GE:
                cmpOp(std::greater_equal<size_t>());
                break;
            case InstructionType::NOT: {
                Value v = pop();
                bool val = v.asBool();
                push(Value(!val));
                ip++;
                break;
            }
            case InstructionType::NEG: {
                Value v = pop();
                int64_t vi = v.asInt();
                push(Value(-vi));
                ip++;
                break;
            }

            case InstructionType::JMZ: {
                Value v = pop();
                bool val = v.asBool();
                if (!val) {
                    ip = inst.intOperand;
                } else {
                    ip++;
                }
                break;
            }
            case InstructionType::PRINT: {
                if (!stack.empty()) {
                    Value v = pop();
                    if (v.getType() == ValueType::INT) {
                        std::cout << v.asInt() << "\n";
                    } else if (v.getType() == ValueType::BOOL) {
                        std::cout << (v.asBool() ? "true" : "false") << "\n";
                    } else if (v.getType() == ValueType::REF) {
                        HeapValue *hv = v.asHeapRef();
                        if (auto sv = dynamic_cast<StringValue *>(hv)) {
                            std::cout << sv->value << "\n";
                        } else if (auto av = dynamic_cast<ArrayValue *>(hv)) {
                            std::cout << "[";
                            for (size_t i = 0; i < av->elements.size(); i++) {
                                if (i > 0) std::cout << ", ";
                                std::cout << av->elements[i].asInt();
                            }
                            std::cout << "]\n";
                        }
                    } else {
                        std::cout << "Unsupported print type\n";
                    }
                } else {
                    std::cout << "\n";
                }
                ip++;
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

                ip++;
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
                push(arr->elements[idx]);
                ip++;
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
                    throw std::runtime_error("Array index out of bounds" + code[ip].toStr());
                arr2->elements[idx2] = val;
                ip++;
                break;
            }
            case InstructionType::ENTER_SCOPE:
                enterScope();
                ip++;
                break;
            case InstructionType::EXIT_SCOPE:
                exitScope();
                ip++;
                break;
            case InstructionType::DUP_TOP: {
                Value v = top();
                push(v);
                ip++;
                break;
            }
            case InstructionType::SWAP: {
                Value a = pop();
                Value b = pop();
                push(a);
                push(b);
                ip++;
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
                throw std::runtime_error("Unknown instruction");
        }
    }
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

        std::string param;
        while (iss >> param) {
            funcInfo.paramNames.push_back(param);
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

    start_adress = ip;
    waitFile = false;
}

std::vector<Instruction> VirtualMachine::getInstructions() {
    return code;
}

void VirtualMachine::optimize(bool optimizeOn = true) {
    isOptimized = optimizeOn;
}

void VirtualMachine::optimizeFunction(const std::string &function_name) {
    auto iter = functionTable.find(function_name);
    if (iter == functionTable.end()) {
        throw std::runtime_error("Function not found: " + code[ip].toStr());
    }

    int64_t address = iter->second.address;
    int64_t next_function_adress = start_adress;
    for (auto &p: functionTable) {
        if (p.second.address > address) {
            next_function_adress = std::min(next_function_adress, p.second.address);
        }
    }

    std::unordered_set<std::string> usedVariables;

    for (int64_t i = next_function_adress - 1; i >= address; i--) {
        if (code[i].op == InstructionType::RET || code[i].op == InstructionType::PUSH_VAR) {
            usedVariables.insert(code[i].strOperand);
        } else if (code[i].op == InstructionType::STORE_VAR) {
            if (usedVariables.find(code[i].strOperand) == usedVariables.end()) {
                int64_t k = 1;
                --i;
                while (code[i].op == InstructionType::ADD
                       || code[i].op == InstructionType::MUL
                       || code[i].op == InstructionType::SUB
                       || code[i].op == InstructionType::DIV
                       || code[i].op == InstructionType::PUSH_INT
                       || code[i].op == InstructionType::PUSH_BOOL
                       || code[i].op == InstructionType::PUSH_STRING
                       || code[i].op == InstructionType::PUSH_VAR) {
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

//    for (int64_t i = address; i < next_function_adress; i++) {
//        std::cout << code[i].toStr() << '\n';
//    }
    optimized_functions.insert(function_name);
}
