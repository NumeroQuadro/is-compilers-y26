#pragma once
#include <string>
#include <utility>

enum class InstructionType {
  PUSH_INT,
  PUSH_BOOL,
  PUSH_STRING,
  PUSH_VAR,
  STORE_VAR,
  ADD,
  SUB,
  MUL,
  DIV,
  EQ,
  NEQ,
  LT,
  LE,
  GT,
  GE,
  NOT,
  NEG,
  JMP,
  JMZ,
  PRINT,
  HALT,
  NEW_ARRAY,
  GET_ELEMENT,
  SET_ELEMENT,
  ENTER_SCOPE,
  EXIT_SCOPE,
  DUP_TOP,
  SWAP
};

struct Instruction {
  InstructionType op;

  int intOperand = 0;
  float floatOperand = 0.0f;
  bool boolOperand = false;
  std::string strOperand;

  explicit Instruction(const InstructionType op) : op(op) {
  }

  Instruction(const InstructionType op, const int i) : op(op), intOperand(i) {
  }

  Instruction(const InstructionType op, const float f) : op(op), floatOperand(f) {
  }

  Instruction(const InstructionType op, const bool b) : op(op), boolOperand(b) {
  }

  Instruction(const InstructionType op, std::string s) : op(op), strOperand(std::move(s)) {
  }
};
