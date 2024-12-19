#pragma once
#include <string>

enum OpCode {
  OP_PUSH,
  OP_LOAD,
  OP_STORE,
  OP_ADD,
  OP_SUB,
  OP_MUL,
  OP_DIV,
  OP_JMP,
  OP_JMZ,
  OP_PRINT,
  OP_HALT
};

struct Instruction {
  OpCode opcode;
  std::string arg;
};

