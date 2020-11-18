/**
 * @file tabela.h
 * @author Eduardo F. Assis
 * @brief 
 * @version 1.0
 * @date 2020-10-19
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef TABELA_H
#define TABELA_H

#include <map>
#include <vector>

using namespace std;

/**
 * @brief Instruction types.
 */
enum class InstType {
    Type1,  /**< Instruction. */
    Type2   /**< Directive. */
};

/**
 * @brief Parameters type
 * 
 */
enum class ParamType {
    EndMem,
    Imediate
};

/**
 * @brief Parameters structure
 * 
 */
struct Param {
    ParamType type;
    int value;
};

/**
 * @brief Used to get instruction information from file.
 * 
 */
struct RawInstruction {
    InstType type;              /**< Instruction type */
    string label;               /**< Used on TS. */
    string instr_name;          /**< String contaning the instruction name. */
    vector<string> operands;    /**< Operands of instruction. */
    int line;                   /**< Line where the instruction was defined. */

    RawInstruction() {
        type = InstType::Type1;
        label.clear();
        instr_name.clear();
        operands.clear();
        line = 0;
    }

    void clear() {
        type = InstType::Type1;
        instr_name.clear();
        label.clear();
        operands.clear();
        line = 0;
    }
};

/**
 * @brief Instruction type.
 * 
 */
struct Instruction {
    size_t qtd_operands;
    int opcode;
    int length;  // bytes

    vector<Param> operands;  // end. mem or imediate

    Instruction(int qtd_operands, int opcode, int length/* , vector<Param> operands */) /* : operands(operands) */{
        this->qtd_operands = qtd_operands;
        this->opcode = opcode;
        this->length = length;
        // this->operands = operands;
    }
};

/**
 * @brief Directive type.
 * 
 */
struct Directive {
    size_t qtd_operands;
    int length;

    vector<Param> operands;

    Directive(int qtd_operands = 0, int length = 0) : qtd_operands(qtd_operands), length(length){};
};

struct AssemblyTables {
    vector<Instruction> text_table;
    vector<Directive> data_table;

    bool empty() {
        return text_table.empty() && data_table.empty();
    }
};

/**
 * @brief Instruction table.
 * 
 */
const map<string, Instruction> TI{
    // {"ADD", Instruction(1, 1, 2, {{ParamType::EndMem, 0}, {ParamType::EndMem, 0}})},      // operands, op, len
    {"ADD", Instruction(1, 1, 2)},      // operands, op, len
    {"SUB", Instruction(1, 2, 2)},
    {"MULT", Instruction(1, 3, 2)},
    {"DIV", Instruction(1, 4, 2)},

    {"JMP", Instruction(1, 5, 2)},
    {"JMPN", Instruction(1, 6, 2)},
    {"JMPP", Instruction(1, 7, 2)},
    {"JMPZ", Instruction(1, 8, 2)},

    {"COPY", Instruction(2, 9, 3)},
    {"LOAD", Instruction(1, 10, 2)},
    {"STORE", Instruction(1, 11, 2)},
    {"INPUT", Instruction(1, 12, 2)},
    {"INPUT", Instruction(1, 12, 2)},
    {"OUTPUT", Instruction(1, 13, 2)},
    {"C_INPUT", Instruction(1, 15, 2)},     // label
    {"C_OUTPUT", Instruction(1, 16, 2)},
    {"S_INPUT", Instruction(2, 19, 3)},     // label, length (max 100)
    {"S_OUTPUT", Instruction(2, 20, 3)},

    {"STOP", Instruction(0, 14, 1)},
};

/**
 * @brief Directive table.
 * 
 */
const map<string, Directive> TD{
    {"EQU", Directive(1, 0)},  // at the beginning
    {"SECTION", Directive(1, 0)},
    {"MACRO", Directive(0, 0)},  // TEXT
    {"ENDMACRO", Directive(0, 0)},
    {"IF", Directive(1, 0)},     // TEXT
    {"SPACE", Directive(1, 1)},  // DATA
    {"CONST", Directive(1, 1)}   // DATA
};

#endif