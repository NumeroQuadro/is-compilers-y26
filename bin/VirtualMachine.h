#pragma once
#include <memory>

#include "Instruction.h"
#include "MyStack.h"

#include <stack>
#include <unordered_map>
#include <vector>

#include "Value.h"

class VirtualMachine {
public:
  explicit VirtualMachine(const std::vector<Instruction> &code);
  void run();
  std::vector<Instruction> getInstructions() const;

private:
  std::vector<Instruction> code;
  std::unordered_map<std::string, Value> variables;
  std::stack<Value> stack;

  std::vector<std::unique_ptr<HeapValue>> heap;

  int ip = 0;

  Value pop();
  Value top();
  void push(const Value &v);

  template<typename Op>
  void binaryOp(Op op);

  template<typename Op>
  void cmpOp(Op op);

  HeapValue* allocHeap(std::unique_ptr<HeapValue> hv);
};
