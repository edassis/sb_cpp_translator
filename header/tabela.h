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
enum InstType {Type1, Type2, Type3};

struct Instruction {
    string name;
    string opcode;
    int qtd_operands;
    int length;     // bytes
    InstType type;
};

struct Diretive {
    string name;
    int qtd_operands;
};

/**
 * @brief Tabela de Símbolos
 * 
 */
map<string, Instruction> TS;

/**
 * @brief Tabela de Diretivas
 * 
 */
map<string, Diretive> TD;

#endif