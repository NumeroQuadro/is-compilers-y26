#pragma once
#include <memory>

#include "Instruction.h"

#include <stack>
#include <vector>
#include <unordered_map>

#include "Scope.h"
#include "Value.h"
#include "CallFrame.h"
#include "FunctionInfo.h"

class VirtualMachine {
public:
  explicit VirtualMachine(const std::vector<Instruction> &code,
                          const std::unordered_map<std::string, FunctionInfo> &functions);

  explicit VirtualMachine(const std::vector<Instruction> &code,
                          const std::unordered_map<std::string, FunctionInfo> &functions,
                          int64_t startPos);

  void run();

  [[nodiscard]] std::vector<Instruction> getInstructions() const;

private:
  Scope *scope_ = nullptr;

  std::unordered_map<std::string, FunctionInfo> functionTable;
  std::vector<Instruction> code;
  std::stack<Value> stack;
  std::vector<std::unique_ptr<HeapValue> > heap;
  std::stack<CallFrame> callStack;

  int64_t ip = 0;

  Value pop();

  Value top();

  void push(const Value &v);

  template<typename Op>
  void binaryOp(Op op);

  template<typename Op>
  void cmpOp(Op op);

  HeapValue *allocHeap(std::unique_ptr<HeapValue> hv);

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
};
