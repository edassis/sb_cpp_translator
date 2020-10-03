#ifndef TABELA_H
#define TABELA_H

#include <map>
#include <vector>

using namespace std;

/**
 * @brief Instruction types.
 * Type1 - OPCODE
 * Type2 - OPCODE ADDRESS
 * Type3 - OPCODE ADDRESS_1 ADDRESS_2
 */
enum InstType { Type1,
                Type2,
                Type3 };

struct Instruction {
    size_t qtd_operands;
    int opcode;
    int length;  // bytes
    
    vector<int> operands;

    Instruction(int qtd_operands, int opcode, int length) {
        this->qtd_operands = qtd_operands;
        this->opcode = opcode;
        this->length = length;
    }
};

struct Diretive {
    size_t qtd_operands;
    int length;

    Diretive(int qtd_operands, int length) : qtd_operands(qtd_operands), length(length){};
};

/**
 * @brief Tabela de Instruções
 * 
 */
const map<string, Instruction> TI {
    {"ADD", Instruction(1, 1, 2)},
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
    {"OUTPUT", Instruction(1, 13, 2)},

    {"STOP", Instruction(0, 14, 1)},
};


/**
 * @brief Tabela de Diretivas
 * 
 */
const map<string, Diretive> TD {
    {"EQU", Diretive(1,0)},         // at the beginning
    {"SECTION", Diretive(1,0)},
    {"MACRO", Diretive(0,0)},       // TEXT
    {"ENDMACRO", Diretive(0,0)},
    {"IF", Diretive(1,0)},          // TEXT
    {"SPACE", Diretive(0,1)},       // DATA
    {"CONST", Diretive(1,1)}        // DATA
};

// map<string, int> TS;
#endif