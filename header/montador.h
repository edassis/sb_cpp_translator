#ifndef MONTADOR_H
#define MONTADOR_H

#include "tabela.h"

void pre_process(ifstream &in_file, ofstream &out_file);

void compile(ifstream &in_file, ofstream &out_file);

int is_number(string &s);

void _obj_one_line(ostream &out_file, vector<Instruction> &text_table, map<int, int> &data_table, map<string, int> &TS);

void _obj_pretty(ostream &out_file, vector<Instruction> &text_table, map<int, int> &data_table, map<string, int> &TS);

#endif