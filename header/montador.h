#ifndef MONTADOR_H
#define MONTADOR_H

#include "tabela.h"

/**
 * @brief Pre-process .asm files removing unnecessary spaces, tabs, new lines. 
 * 
 * @param in_file   Get instruction information from here (.asm file).
 * @param out_file  Put result here (.PRE file).
 */
void pre_process(ifstream &in_file, ofstream &out_file);

/**
 * @brief Generate binary file .OBJ doing multiple checks
 * 
 * @param in_file  Get instruction information from here (.PRE file).
 * @param out_file Put result here (.OBJ file).
 * @param mode     0 - One line output; 1 - Tabulated output.
 */
void compile(ifstream &in_file, ofstream &out_file, int mode = 0);

/**
 * @brief Check if a given string is a number.
 * 
 * @param s String to check.
 * @return true
 * @return false 
 */
bool is_number(string &s);

/**
 * @brief Function that prints opcode, params in one line into the \p out_file.
 * 
 * @param out_file      Destination (.OBJ file).
 * @param text_table    Table contaning instructions from TEXT section.
 * @param data_table    Table contaning variables from DATA section.
 * @param TS            Table of symbols.
 */
void _obj_one_line(ostream &out_file, vector<Instruction> &text_table, map<int, int> &data_table, map<string, int> &TS);

/**
 * @brief Function that prints opcode, params in a tabulated form into the \p out_file.
 * 
 * @param out_file      Destination (.OBJ file).
 * @param text_table    Table contaning instructions from TEXT section.
 * @param data_table    Table contaning variables from DATA section.
 * @param TS            Table of symbols.
 */
void _obj_pretty(ostream &out_file, vector<Instruction> &text_table, map<int, int> &data_table, map<string, int> &TS);

/**
 * @brief Function that go through every char from \p in_file making valid tokens.
 * 
 * Do some checks (lexical and syntactic) associeted of tokens validity and
 * instructions/directives formation.
 * 
 * @param in_file   Scan tokens from here (.asm file).
 * @param raw_instr Put parsed tokens here.
 * @param line      Current line of \p in_file.
 * @return true     If was able to get a new instruction.
 * @return false    If not.
 */
bool _get_instr(ifstream &in_file, RawInstruction &raw_instr, int &line);

#endif