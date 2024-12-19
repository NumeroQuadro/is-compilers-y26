#pragma once
#include "Instruction.h"
#include "MyStack.h"

#include <stack>
#include <unordered_map>
#include <vector>

class VirtualMachine {
private:
  bool is_running = true;

  int _ip;
  std::vector<Instruction> _code;
  MyStack<int> _stack;
  std::unordered_map<std::string,int> _memory;
public:
  explicit VirtualMachine(const std::vector<Instruction>& code)
      : _ip(0), _code(code) {}

  void run();
};
