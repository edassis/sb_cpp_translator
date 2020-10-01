#ifndef _TABELA_H_
#define _TABELA_H_

#include <map>

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
    string name;
    int opcode;
    int qtd_operands;
    int length;  // bytes

    Instruction(string name, int opcode, int qtd_operands, int length) {
        this->name = name;
        this->opcode = opcode;
        this->qtd_operands = qtd_operands;
        this->length = length;
    }
};

struct Diretive {
    int qtd_operands;
    int length;

    Diretive(int qtd_operands, int length) : qtd_operands(qtd_operands), length(length){};
};

/**
 * @brief Tabela de Instruções
 * 
 */
struct TI {
    Instruction ADD, SUB, MULT, DIV;
    Instruction JMP, JMPN, JMPP, JMPZ;
    Instruction COPY, LOAD, STORE;
    Instruction INPUT, OUTPUT, STOP;

    TI() :  ADD("ADD", 1, 1, 2), SUB("SUB", 1, 2, 2), MULT("MULT", 1, 3, 2), DIV("DIV", 1, 4, 2),
            JMP("JMP", 1, 5, 2), JMPN("JMPN", 1, 6, 2), JMPP("JMPP", 1, 7, 2), JMPZ("JMPZ", 1, 8, 2),
            COPY("COPY", 2, 9, 3), LOAD("LOAD", 1, 10, 2), STORE("STORE", 1, 11, 2), INPUT("INPUT", 1, 12, 2), OUTPUT("OUTPUT", 1, 13, 2),
            STOP("STOP", 0, 14, 1) {}
};

/**
 * @brief Tabela de Diretivas
 * 
 */
// struct TD {
//     Diretive SECTION, SPACE, CONST, EQU, IF, MACRO, ENDMACRO; 
//     TD() : SECTION(1, 0), SPACE(0, 1), CONST(1, 1), EQU(1, 0), IF(1, 0), MACRO(0, 0), ENDMACRO(0, 0) {} 
// };

const map<string, Diretive> TD {
    {"SECTION", Diretive(1,0)},
    {"SPACE", Diretive(0,1)},
    {"CONST", Diretive(1,1)},
    {"EQU", Diretive(1,0)},
    {"IF", Diretive(1,0)},
    {"MACRO", Diretive(0,0)},
    {"ENDMACRO", Diretive(0,0)}
};

map<string, int> TS;

#endif