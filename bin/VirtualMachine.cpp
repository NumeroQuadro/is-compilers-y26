
#include "VirtualMachine.h"

#include <iostream>
#include <ostream>

void VirtualMachine::run() {
  while (is_running) {
    const auto& instr = _code[_ip];
    switch (instr.opcode) {
      case OP_PUSH: {
        int val = std::stoi(instr.arg);
        _stack.push(val);
        ++_ip;
        break;
      }
      case OP_LOAD: {
        int val = _memory[instr.arg];
        _stack.push(val);
        ++_ip;
        break;
      }
      case OP_STORE: {
        int val = _stack.topAndPop();

        _memory[instr.arg] = val;
        ++_ip;
        break;
      }
      case OP_ADD: {
        const int a = _stack.topAndPop();
        const int b = _stack.topAndPop();
        _stack.push(a + b);
        ++_ip;
        break;
      }
      case OP_SUB: {
        const int a = _stack.topAndPop();
        const int b = _stack.topAndPop();
        _stack.push(a - b);
        ++_ip;
        break;
      }
      case OP_MUL: {
        const int a = _stack.topAndPop();
        const int b = _stack.topAndPop();
        _stack.push(a * b);
        ++_ip;
        break;
      }
      case OP_DIV: {
        const int a = _stack.topAndPop();
        const int b = _stack.topAndPop();
        _stack.push(a * b);
        ++_ip;
        break;
      }
      case OP_JMP: {
        _ip = std::stoi(instr.arg);
        break;
      }
      case OP_JMZ: {
        int cond = _stack.topAndPop();
        if (cond == 0) {
          _ip = std::stoi(instr.arg);
        } else {
          _ip++;
        }
        break;
      }
      case OP_PRINT: {
        int val = _stack.topAndPop();
        std::cout << val << std::endl;
        _ip++;
        break;
      }
      case OP_HALT: {
        return;
      }
    }
  }
}
