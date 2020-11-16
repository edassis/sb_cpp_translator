/**
 * @file montador.h
 * @author Eduardo F. Assis
 * @brief 
 * @version 1.0
 * @date 2020-10-19
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#ifndef MONTADOR_H
#define MONTADOR_H

#include "tabela.h"

/**
 * @brief Pre-process .asm files removing unnecessary spaces, tabs, new lines. 
 * 
 * Only checks for EQU directive usage.
 * 
 * @param in_file   Get instruction information from here (.asm file).
 * @param out_file  Put result here (.PRE file).
 */
void pre_process_basic(ifstream &in_file, ofstream &out_file);

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
 * @param mode     0 - One line output; 1 - Tabulated output; 2 - x86 assembly.
 */
bool assembly(ifstream &in_file, ofstream &out_file, int mode = 0);

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
 * @param data_table    Table contaning directives from DATA section.
 */
void _obj_one_line(ostream &out_file, vector<Instruction> &text_table, vector<Directive> &data_table);

/**
 * @brief Function that go through every char from \p in_file making valid tokens.
 * 
 * Only checks if a given instruction/directive is valid, put one command per
 * line (label - command - params).
 * 
 * @param in_file   Scan tokens from here (.asm file).
 * @param raw_instr Put parsed tokens here.
 * @param line      Current line of \p in_file.
 * @return \b True     If was able to get a new instruction.
 * @return \b False    If not.
 */
bool _get_instr_basic(ifstream &in_file, RawInstruction &raw_instr, int &line);

/**
 * @brief Function that prints opcode, params in a tabulated form into the \p out_file.
 * 
 * @param out_file      Destination (.OBJ file).
 * @param text_table    Table contaning instructions from TEXT section.
 * @param data_table    Table contaning directives from DATA section.
 */
void _obj_pretty(ostream &out_file, vector<Instruction> &text_table, vector<Directive> &data_table);

/**
 * @brief Function that go through every char from \p in_file making valid tokens.
 * 
 * Do some checks (lexical and syntactic) associeted of tokens validity and
 * instructions/directives formation.
 * 
 * @param in_file   Scan tokens from here (.asm file).
 * @param raw_instr Put parsed tokens here.
 * @param line      Current line of \p in_file.
 * @return \b True     If was able to get a new instruction.
 * @return \b False    If not.
 */
bool _get_instr(ifstream &in_file, RawInstruction &raw_instr, int &line);

/**
 * @brief Translates the pre-processed file in our created assembly to x86 NASM assembly.
 * 
 * @param in_file   .PRE file
 * @param out_file  The translated file (.asm).
 * @return \b True  If translated with success.
 * @return \b False Case not.
 */
bool translate_x86(ifstream &in_file, ofstream &out_file);

/**
 * @brief Used to create the structure containing the operations analyzed and
 * recognized from the input file (Compile step). 
 * 
 * @param in_file   .PRE file
 * @return AssemblyTables   Structure containing the data to generate the binary file.
 */
AssemblyTables generate_tables(ifstream &in_file);
#endif