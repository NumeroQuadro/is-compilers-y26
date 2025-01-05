#pragma once

#include <string>
#include <utility>
#include <sstream>
#include <iomanip>
#include <vector>

enum class InstructionType {
    PUSH_INT,
    PUSH_DOUBLE,
    PUSH_BOOL,
    PUSH_STRING,
    PUSH_VAR,
    STORE_VAR,
    ADD,
    SUB,
    MUL,
    DIV,
    DIV_REM,
    AND,
    OR,
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
    SWAP,
    CALL,
    RET
};

struct Instruction {
    InstructionType op;


  int64_t intOperand = 0;
  double doubleOperand = 0.0;
  bool boolOperand = false;
  std::string strOperand;

    explicit Instruction(const InstructionType op) : op(op) {
    }

    Instruction(const InstructionType op, const int64_t i) : op(op), intOperand(i) {
    }

  Instruction(const InstructionType op, const double f) : op(op), doubleOperand(f) {
  }

    Instruction(const InstructionType op, std::string s) : op(op), strOperand(std::move(s)) {
    }

    Instruction(const InstructionType op, const bool b) : op(op), boolOperand(b) {
    }

    std::string toStr() {
        std::ostringstream oss;

        switch (op) {
            case InstructionType::PUSH_INT:
                oss << "PUSH_INT";
                break;
            case InstructionType::PUSH_BOOL:
                oss << "PUSH_BOOL";
                break;
            case InstructionType::PUSH_STRING:
                oss << "PUSH_STRING";
                break;
            case InstructionType::PUSH_VAR:
                oss << "PUSH_VAR";
                break;
            case InstructionType::STORE_VAR:
                oss << "STORE_VAR";
                break;
            case InstructionType::ADD:
                oss << "ADD";
                break;
            case InstructionType::SUB:
                oss << "SUB";
                break;
            case InstructionType::MUL:
                oss << "MUL";
                break;
            case InstructionType::DIV:
                oss << "DIV";
                break;
            case InstructionType::EQ:
                oss << "EQ";
                break;
            case InstructionType::NEQ:
                oss << "NEQ";
                break;
            case InstructionType::LT:
                oss << "LT";
                break;
            case InstructionType::LE:
                oss << "LE";
                break;
            case InstructionType::GT:
                oss << "GT";
                break;
            case InstructionType::GE:
                oss << "GE";
                break;
            case InstructionType::NOT:
                oss << "NOT";
                break;
            case InstructionType::NEG:
                oss << "NEG";
                break;
            case InstructionType::JMP:
                oss << "JMP";
                break;
            case InstructionType::JMZ:
                oss << "JMZ";
                break;
            case InstructionType::PRINT:
                oss << "PRINT";
                break;
            case InstructionType::HALT:
                oss << "HALT";
                break;
            case InstructionType::NEW_ARRAY:
                oss << "NEW_ARRAY";
                break;
            case InstructionType::GET_ELEMENT:
                oss << "GET_ELEMENT";
                break;
            case InstructionType::SET_ELEMENT:
                oss << "SET_ELEMENT";
                break;
            case InstructionType::ENTER_SCOPE:
                oss << "ENTER_SCOPE";
                break;
            case InstructionType::EXIT_SCOPE:
                oss << "EXIT_SCOPE";
                break;
            case InstructionType::DUP_TOP:
                oss << "DUP_TOP";
                break;
            case InstructionType::SWAP:
                oss << "SWAP";
                break;
            case InstructionType::CALL:
                oss << "CALL";
                break;
            case InstructionType::RET:
                oss << "RET";
                break;
            case InstructionType::DIV_REM:
                oss << "DIV_REM";
                break;
            case InstructionType::AND:
                oss << "AND";
                break;
            case InstructionType::OR:
                oss << "OR";
                break;
        }
        oss << " " << std::to_string(doubleOperand) << " " << std::to_string(intOperand) << " " << strOperand << " " << (boolOperand ? "true" : "false");

        return oss.str();
    }

    static Instruction fromStr(const std::string &str) {
        auto tokens = split(str);

        if (tokens.size() < 4) {
            throw std::runtime_error("Invalid instruction format: " + str);
        }

        const std::string &opStr = tokens[0];
        InstructionType operation;

        if (opStr == "PUSH_INT") operation = InstructionType::PUSH_INT;
        else if (opStr == "PUSH_BOOL") operation = InstructionType::PUSH_BOOL;
        else if (opStr == "PUSH_STRING") operation = InstructionType::PUSH_STRING;
        else if (opStr == "PUSH_VAR") operation = InstructionType::PUSH_VAR;
        else if (opStr == "STORE_VAR") operation = InstructionType::STORE_VAR;
        else if (opStr == "ADD") operation = InstructionType::ADD;
        else if (opStr == "SUB") operation = InstructionType::SUB;
        else if (opStr == "MUL") operation = InstructionType::MUL;
        else if (opStr == "DIV") operation = InstructionType::DIV;
        else if (opStr == "EQ") operation = InstructionType::EQ;
        else if (opStr == "NEQ") operation = InstructionType::NEQ;
        else if (opStr == "LT") operation = InstructionType::LT;
        else if (opStr == "LE") operation = InstructionType::LE;
        else if (opStr == "GT") operation = InstructionType::GT;
        else if (opStr == "GE") operation = InstructionType::GE;
        else if (opStr == "NOT") operation = InstructionType::NOT;
        else if (opStr == "NEG") operation = InstructionType::NEG;
        else if (opStr == "JMP") operation = InstructionType::JMP;
        else if (opStr == "JMZ") operation = InstructionType::JMZ;
        else if (opStr == "PRINT") operation = InstructionType::PRINT;
        else if (opStr == "HALT") operation = InstructionType::HALT;
        else if (opStr == "NEW_ARRAY") operation = InstructionType::NEW_ARRAY;
        else if (opStr == "GET_ELEMENT") operation = InstructionType::GET_ELEMENT;
        else if (opStr == "SET_ELEMENT") operation = InstructionType::SET_ELEMENT;
        else if (opStr == "ENTER_SCOPE") operation = InstructionType::ENTER_SCOPE;
        else if (opStr == "EXIT_SCOPE") operation = InstructionType::EXIT_SCOPE;
        else if (opStr == "DUP_TOP") operation = InstructionType::DUP_TOP;
        else if (opStr == "SWAP") operation = InstructionType::SWAP;
        else if (opStr == "CALL") operation = InstructionType::CALL;
        else if (opStr == "RET") operation = InstructionType::RET;
        else if (opStr == "AND") operation = InstructionType::AND;
        else if (opStr == "OR") operation = InstructionType::OR;
        else if (opStr == "DIV_REM") operation = InstructionType::DIV_REM;
        else throw std::runtime_error("Unknown instruction type: " + opStr);

        double doubleOperand = std::stod(tokens[1]);
        int64_t intOperand = std::stoll(tokens[2]);
        bool boolOperand = (tokens[4] == "true");

        std::string strOperand = tokens[3];

        Instruction instruction(operation);
        instruction.doubleOperand = doubleOperand;
        instruction.intOperand = intOperand;
        instruction.boolOperand = boolOperand;
        instruction.strOperand = strOperand;

        return instruction;
    }

    static std::vector<std::string> split(const std::string &str, char delimiter = ' ') {
        std::vector<std::string> tokens;
        size_t start = 0;
        size_t end = str.find(delimiter);

        while (end != std::string::npos) {
            tokens.push_back(str.substr(start, end - start));
            start = end + 1;
            end = str.find(delimiter, start);
        }

        tokens.push_back(str.substr(start));
        return tokens;
    }

    bool operator==(const Instruction &other) const {
        if (op != other.op) return false;

        return (intOperand == other.intOperand) && (boolOperand == other.boolOperand) &&
               (strOperand == other.strOperand);
    }

    bool operator!=(const Instruction &other) const {
        return !(*this == other);
    }
};
