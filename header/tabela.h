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
    Immediate
};

/**
 * @brief Parameters structure
 * 
 */
struct Param {
    ParamType type = ParamType::EndMem;
    int value = 0;
    int offset = 0;
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
    string label;
    string name;
    size_t qtd_operands;
    int opcode;
    int length;  // bytes

    vector<Param> operands;  // end. mem or imediate

    Instruction(string name, int qtd_operands, int opcode, int length/* , vector<Param> operands */) /* : operands(operands) */{
        this->name = name;
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
    string label;
    string name;
    size_t qtd_operands;
    int length;

    vector<Param> operands;

    Directive(string name = "", int qtd_operands = 0, int length = 0) : name(name), qtd_operands(qtd_operands), length(length){};
};

struct AssemblyTables {
    vector<Instruction> text_table;
    vector<Directive> data_table;
    map<string, int> TS;

    bool empty() {
        return text_table.empty() && data_table.empty() && !TS.size();
    }
};

/**
 * @brief Instruction table.
 * 
 */
const map<string, Instruction> TI{
    // {"ADD", Instruction(1, 1, 2, {{ParamType::EndMem, 0}, {ParamType::EndMem, 0}})},      // operands, op, len
    {"ADD", Instruction("ADD"   , 1, 1, 2)},      // operands, op, len
    {"SUB", Instruction("SUB"   , 1, 2, 2)},
    {"MULT", Instruction("MULT" , 1, 3, 2)},
    {"DIV", Instruction("DIV"   , 1, 4, 2)},

    {"JMP", Instruction("JMP"   , 1, 5, 2)},
    {"JMPN", Instruction("JMPN" , 1, 6, 2)},
    {"JMPP", Instruction("JMPP" , 1, 7, 2)},
    {"JMPZ", Instruction("JMPZ" , 1, 8, 2)},

    {"COPY", Instruction("COPY"         , 2, 9, 3)},
    {"LOAD", Instruction("LOAD"         , 1, 10, 2)},
    {"STORE", Instruction("STORE"       , 1, 11, 2)},

    {"INPUT", Instruction("INPUT"       , 1, 12, 2)},
    {"OUTPUT", Instruction("OUTPUT"     , 1, 13, 2)},
    {"C_INPUT", Instruction("C_INPUT"   , 1, 15, 2)},     // label
    {"C_OUTPUT", Instruction("C_OUTPUT" , 1, 16, 2)},
    {"S_INPUT", Instruction("S_INPUT"   , 2, 19, 3)},     // label, length (max 100)
    {"S_OUTPUT", Instruction("S_OUTPUT" , 2, 20, 3)},

    {"STOP", Instruction("STOP", 0, 14, 1)},
};

/**
 * @brief Directive table.
 * 
 */
const map<string, Directive> TD{
    {"EQU", Directive("EQU",            1, 0)},  // at the beginning
    {"SECTION", Directive("SECTION",    1, 0)},
    {"MACRO", Directive("MACRO",        0, 0)},  // TEXT
    {"ENDMACRO", Directive("ENDMACRO",  0, 0)},
    {"IF", Directive("IF",              1, 0)},     // TEXT
    {"SPACE", Directive("SPACE",        1, 1)},  // DATA
    {"CONST", Directive("CONST",        1, 1)}   // DATA
};

// command, output
const map<string, string> TranslationTable {    // * - operands
    // Instructions
    {
        "ADD",
            "\tadd eax, dword *\n"    // [var]
    },
    {
        "SUB",
            "\tsub eax, dword *\n"
    },
    {
        "MULT",
            "\tcdq\n"
            "\tmov ebx, dword *\n"
            "\timul ebx\n"
            "\tjo Overflow\n"
    },
    {
        "DIV",
            "\tcdq\n"
            "\tmov ebx, dword *\n"
            "\tidiv ebx\n"
    },
    { 
        "JMP",
            "\tjmp *\n"       // label
    },
    { 
        "JMPN",
            "\tcmp eax, 0\n"
            "\tjl *\n"
    },
    { 
        "JMPP",
            "\tcmp eax, 0\n"
            "\tjg *\n"
    },
    {
        "JMPZ",
            "\tcmp eax, 0\n"
            "\tje *\n"        
    },
    {
        "COPY",
            "\tmov ebx, dword *\n"    // param1 - [src var]
            "\tmov dword *, ebx\n"    // param2 - [dst var]
    },
    {
        "LOAD",
            "\tmov eax, dword *\n"    // [var]
    },
    {
        "STORE",
            "\tmov dword *, eax\n"
    },
    {
        "INPUT",
            "\tpush *\n"                // var name
            "\tcall LerInteiro\n"
    },
    {
        "OUTPUT",
            "\tpush *\n"
            "\tcall EscreverInteiro\n"
    },
    {
        "C_INPUT",
            "\tpush *\n"
            "\tcall LerChar\n"
    },
    {
        "C_OUTPUT",
            "\tpush *\n"
            "\tcall EscreverChar\n"
    },
    {
        "S_INPUT",
            "\tpush *\n"          // var name
            "\tpush *\n"          // len
            "\tcall LerString\n"
    },
    {
        "S_OUTPUT",
            "\tpush *\n"
            "\tpush *\n"
            "\tcall EscreverString\n"
    },
    {
        "STOP",
            "\tmov eax, 1\n"
            "\tmov ebx, 0\n"
            "\tint 80h\n"
    },

    // DIRECTIVES
    {
        "SPACE",
            "resd *\n"      // qtd | <label> ...
    },
    {
        "CONST",
            "dd *\n"        // initial value | <label> ...
    }
};
#endif