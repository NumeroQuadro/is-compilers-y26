#pragma once
#include <memory>

#include "Instruction.h"

#include <stack>
#include <vector>

#include "Scope.h"
#include "Value.h"
#include "CallFrame.h"

class VirtualMachine {
public:
  explicit VirtualMachine(const std::vector<Instruction> &code);

  void run();

  [[nodiscard]] std::vector<Instruction> getInstructions() const;

private:
  Scope *scope_ = nullptr;

  std::vector<Instruction> code;
  std::stack<Value> stack;
  std::vector<std::unique_ptr<HeapValue> > heap;
  std::stack<CallFrame> callStack;

  int ip = 0;

  Value pop();

  Value top();

  void push(const Value &v);

  template<typename Op>
  void binaryOp(Op op);

  template<typename Op>
  void cmpOp(Op op);

  HeapValue *allocHeap(std::unique_ptr<HeapValue> hv);

  Value loadVar(const std::string &name);

  void createVar(const std::string &name, const Value &val);

  void storeVar(const std::string &name, const Value &val);

  void enterScope();

  void exitScope();
};
