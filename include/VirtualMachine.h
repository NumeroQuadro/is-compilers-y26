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

class GarbageCollector;

class VirtualMachine {
public:
  VirtualMachine();

  VirtualMachine(GarbageCollector *garbageCollector);

  explicit VirtualMachine(const std::vector<Instruction> &code,
                          const std::unordered_map<std::string, FunctionInfo> &functions,
                          GarbageCollector *garbageCollector);

  explicit VirtualMachine(const std::vector<Instruction> &code,
                          const std::unordered_map<std::string, FunctionInfo> &functions,
                          GarbageCollector *garbageCollector,
                          int64_t startPos);

  ~VirtualMachine();

  void run();

  size_t getRunnedOperationsCount();

  Scope *getCurrentScope() const;

  std::stack<Value> &getStackRef();

  [[nodiscard]] std::vector<Instruction> getInstructions() const;

  void fromFile(const std::string &path);

  void optimize(bool optimizeOn);

  size_t getHeapSize() const;

private:
  Scope *scope_ = nullptr;
  GarbageCollector *gc;

  std::unordered_map<std::string, Value> functionsCallCache;
  std::unordered_map<std::string, FunctionInfo> functionTable;
  std::vector<Instruction> code;
  std::stack<Value> stack;
  std::vector<HeapValue *> heapRefs;
  std::stack<CallFrame> callStack;
  bool waitFile = true;
  bool isOptimized = false;
  std::unordered_set<std::string> optimized_functions;
  int64_t start_address{};
  size_t runnedOperationsCount = 0;
  std::stack<FunctionCacheParams> function_cache_params_stack;

  int64_t ip = 0;

  Value pop();

  Value top() const;

  void push(const Value &v);

  void partialGC(Scope *scope);

  void markValue(const Value &v);

  void markScope(Scope *scope);

  HeapValue *allocHeap(std::unique_ptr<HeapValue> hv) const;

  Value loadVar(const std::string &name) const;

  void createVar(const std::string &name, const Value &val) const;

  void storeVar(const std::string &name, const Value &val) const;

  void enterScope();

  void exitScope();

  bool hasPrintOrCall(const std::string &funcName) const;

  void doCall(const std::string &funcName);

  void doRet();

  void builtInPushBack();

  void builtInPopBack();

  void builtInSize();

  void optimizeFunction(const std::string &function_name);

  void optimizeMain();

 void foldConstants(int64_t start, int64_t finish);

 void deleteDeadCode(int64_t start, int64_t finish);
};
