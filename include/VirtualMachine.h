#pragma once

#include <memory>

#include "Instruction.h"

#include <stack>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <fstream>

#include "Scope.h"
#include "Value.h"
#include "CallFrame.h"
#include "FunctionInfo.h"

class VirtualMachine {
public:
  VirtualMachine();

  explicit VirtualMachine(const std::vector<Instruction> &code,
                          const std::unordered_map<std::string, FunctionInfo> &functions);

  explicit VirtualMachine(const std::vector<Instruction> &code,
                          const std::unordered_map<std::string, FunctionInfo> &functions,
                          int64_t startPos);

  void run();

  void gc();

  [[nodiscard]] std::vector<Instruction> getInstructions() const;

  void fromFile(const std::string &path);

  std::vector<Instruction> getInstructions();

  void optimize(bool optimizeOn);

private:
  Scope *scope_ = nullptr;

  std::unordered_map<std::string, FunctionInfo> functionTable;
  std::vector<Instruction> code;
  std::stack<Value> stack;
  std::vector<std::unique_ptr<HeapValue> > heap;
  std::stack<CallFrame> callStack;
  bool waitFile = true;
  bool isOptimized = false;
  std::unordered_set<std::string> optimized_functions;
  int64_t start_address{};

  int64_t ip = 0;

  Value pop();

  Value top();

  void push(const Value &v);

  void unmarkAll();
  void markAll();
  void sweep();

  void markValue(const Value &v);
  void markScope(Scope* scope);

  HeapValue *allocHeap(HeapValue *hv);

  Value loadVar(const std::string &name) const;

  void createVar(const std::string &name, const Value &val) const;

  void storeVar(const std::string &name, const Value &val) const;

  void enterScope();

  void exitScope();

  void doCall(const std::string &funcName);

  void doRet();

  void builtInPushBack();

  void builtInPopBack();

  void builtInSize();

  void optimizeFunction(const std::string &function_name);
};
