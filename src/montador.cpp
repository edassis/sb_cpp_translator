#include "montador.h"

#include <string.h>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "tabela.h"

#define LINE_LENGTH 256
#define DELIMETERS " "

using namespace std;

int is_number(string &s) {
    bool is_number = true;
    for (size_t i = 0; i < s.length(); i++) {
        if (!isdigit(s[i])) {
            is_number = false;
            break;
        }
    }

    return is_number;
}

void _obj_one_line(ostream &out_file, vector<Instruction> &text_table, map<int, int> &data_table, map<string, int> &TS) {
    for (Instruction &instr : text_table) {
        out_file << instr.opcode << ' ';

        for (auto &e : instr.operands) {
            out_file << e << ' ';
        }
    }

    for (auto &e : TS) {
        if (data_table.count(e.second) > 0) {
            out_file << data_table[e.second] << ' ';
        }
    }

    out_file << endl;
}

void _obj_pretty(ostream &out_file, vector<Instruction> &text_table, map<int, int> &data_table, map<string, int> &TS) {
    int end_counter = 0;

    for (Instruction &instr : text_table) {
        string end = "End " + to_string(end_counter) + ":";
        out_file << setw(10) << left << end << setw(6) << right << instr.opcode;

        for (auto &e : instr.operands) {
            out_file << setw(6) << right << e;
        }
        out_file << endl;
        end_counter += instr.length;
    }

    for (auto &e : TS) {
        string end = "End " + to_string(end_counter) + ":";
        if (data_table.count(e.second) > 0) {
            out_file << setw(10) << left << end << setw(6) << right << data_table[e.second] << endl;
            ;
            end_counter++;
        }
    }
}

void pre_process(ifstream &in_file, ofstream &out_file) {
    char line[LINE_LENGTH];
    vector<string> token;
    int line_count = 0;

    enum class MajorState { Equ,
                            Text,
                            Data,
                            PassThrough };
    MajorState currentMajorState = MajorState::Equ;
    MajorState previousMajorState = currentMajorState;

    enum class TextState { Get,
                           Skip };
    TextState currentTextState = TextState::Get;

    map<string, int> equ_table;
    string output_line;

    while (!in_file.eof() && !in_file.bad()) {
        if (currentMajorState == MajorState::PassThrough) {
            currentMajorState = previousMajorState;
        }
        output_line.clear();

        // Get line
        in_file.getline(line, LINE_LENGTH);
        line_count++;

        for (int i = 0; i < LINE_LENGTH; i++) {
            line[i] = toupper(line[i]);  // Uppercase

            if (line[i] == ';') {  // Removing coments
                line[i] = '\0';
                break;
            }
        }

        // Tokenize
        token.clear();
        char *pch;

        if (line[0] != '\0') {
            pch = strtok(line, DELIMETERS);
            if (pch == NULL) {
                cout << "Erro! Nao foi possivel formar o token "
                     << "(linha " << line_count << ")." << endl;
                currentMajorState = MajorState::PassThrough;
            }
            while (pch != NULL) {
                bool valid_token = true;
                for (size_t i = 0; i < strlen(pch); i++) {
                    // Check characters
                    if (!isalnum(pch[i]) && pch[i] != ':' && pch[i] != '_' && pch[i] != ',') {
                        cout << "Erro! Nao foi possivel formar o token "
                             << "(linha " << line_count << ")." << endl;
                        valid_token = false;
                        currentMajorState = MajorState::PassThrough;
                        break;
                    }
                }
                if (valid_token)
                    token.push_back(pch);

                pch = strtok(NULL, DELIMETERS);
            }
        }

        // Major state transition
        if (token.front() == "SECTION") {
            if (token[1] == "TEXT" && token.size() == 2) {
                currentMajorState = MajorState::Text;
                previousMajorState = currentMajorState;
                output_line = "SECTION TEXT\n";
            } else if (token[1] == "DATA" && token.size() == 2) {
                currentMajorState = MajorState::Data;
                previousMajorState = currentMajorState;
                output_line = "SECTION DATA\n";
            } else {
                cout << "Erro! Secao invalida "
                     << "(linha " << line_count << ")." << endl;
            }

            currentMajorState = MajorState::PassThrough;
        }

        // States
        if (currentMajorState == MajorState::Equ) {
            if (!token.empty() && token[0].find(":") != string::npos) {
                string label = token.front();
                label.pop_back();

                if (token[0].size() < 50 && token[1] == "EQU") {  // Check command
                    bool is_number = true;                        // Check number
                    for (char &c : token[2]) {
                        if (!isdigit(c)) {
                            is_number = false;
                            break;
                        }
                    }
                    if (is_number) {
                        int number = stoi(token[2]);

                        if (token.size() > 3) {
                            cout << "Erro! Instrucoes com a quantidade de operando invalido"
                                 << "(linha " << line_count << ")." << endl;
                        } else if (equ_table.count(label) > 0) {
                            cout << "Erro! Rotulo repetido "
                                 << "(linha " << line_count << ")." << endl;
                        } else {
                            equ_table[label] = number;
                        }
                    } else {
                        cout << "Erro! Instrucoes com o tipo de operando invalido"
                             << "(linha " << line_count << ")." << endl;
                    }
                } else {
                    cout << "Erro! Diretiva invalida "
                         << "(linha " << line_count << ")." << endl;
                }
            }
            // não é rótulo e não está em TI
            else if (!token.empty() && TI.count(token[0]) == 0) {  // Found invalid diretive
                cout << "Erro! Diretiva invalida "
                     << "(linha " << line_count << ")." << endl;
            }
        } else if (currentMajorState == MajorState::Text) {
            if (token[0] == "IF") {
                if (equ_table.count(token[1]) > 0) {  // Search equ label
                    if (equ_table[token[1]] == 1) {
                        currentTextState = TextState::Get;
                    } else {
                        currentTextState = TextState::Skip;
                    }
                } else {
                    cout << "Erro! Rotulo ausente "
                         << "(linha " << line_count << ")." << endl;
                }
            } else if (currentTextState == TextState::Get) {  // Nao checa instrucoes em text
                for (size_t i = 0; i < token.size(); i++) {
                    output_line += token[i];
                    if (i + 1 < token.size()) output_line += ' ';
                }
                output_line += '\n';
            } else if (currentTextState == TextState::Skip) {
                output_line.clear();
                currentTextState = TextState::Get;
            }
        } else if (currentMajorState == MajorState::Data) {  // Nao checa rotulos em data
            for (size_t i = 0; i < token.size(); i++) {
                output_line += token[i];
                if (i + 1 < token.size()) output_line += ' ';
            }
            output_line += '\n';
        }

        if (!output_line.empty())
            out_file << output_line;
    }
}

void compile(ifstream &in_file, ofstream &out_file) {
    enum class MajorState { FirstPass,
                            SecondPass };
    MajorState currentMajorState = MajorState::FirstPass;

    enum class Section { None,
                         Text,
                         Data };
    Section currentSection = Section::None;

    bool stop = false;
    int line_count = 0;
    int end_count = 0;

    bool found_text = false;

    // Tabela de Símbolos
    map<string, int> TS;

    // each: end. mem, opcode, param1 (end. mem), ...
    vector<Instruction> text_table;

    // end. mem / content
    map<int, int> data_table;

    // bool found_data = false;

    while (!stop) {  // multiple passes into the file
        while (!in_file.eof() && !in_file.bad()) {
            char line[LINE_LENGTH];

            // Line
            in_file.getline(line, LINE_LENGTH);
            line_count++;

            // Tokenize
            vector<string> token;
            char *pch;
            if (strlen(line) > 0) {
                pch = strtok(line, " ,");
                while (pch != NULL) {
                    token.push_back(pch);

                    pch = strtok(NULL, " ,");
                }
            }

            // specifing
            string label;
            string command;
            vector<string> param;

            for (string &e : token) {
                if (label.empty() && (e.find(":") != string::npos)) {
                    label = e;
                    label.pop_back();
                } else if (command.empty() && (TI.count(e) > 0 || TD.count(e) > 0)) {
                    command = e;
                } else {
                    param.push_back(e);
                }
            }

            // Check section
            if (!command.empty() && token[0] == "SECTION") {
                if (param.front() == "TEXT") {
                    found_text = true;
                    currentSection = Section::Text;
                } else if (param.front() == "DATA") {
                    currentSection = Section::Data;
                } else {
                    cout << "Erro! Secao invalida "
                         << "(linha " << line_count << ")." << endl;
                }

                command.clear();
                param.clear();
            }

            if (currentMajorState == MajorState::FirstPass) {  // make TS
                if (currentSection == Section::Text) {
                    if (!label.empty()) {
                        if (TS.count(label) == 0) {
                            TS[label] = end_count;
                        } else {
                            cout << "Erro! Rotulo Repetido "
                                 << "(linha " << line_count << ")." << endl;
                        }
                    }
                } else if (currentSection == Section::Data) {
                    if (!label.empty()) {
                        if (TS.count(label) == 0) {
                            if (command == "SPACE") {
                                TS[label] = end_count;
                                data_table[end_count] = 0;
                            } else if (command == "CONST") {
                                TS[label] = end_count;

                                if (!param.empty() && is_number(param.front())) {
                                    data_table[end_count] = stoi(param.front());
                                } else {
                                    cout << "Erro! Diretiva com operando invalido "
                                         << "(linha " << line_count << ")." << endl;
                                }
                            }
                        } else {
                            cout << "Erro! Rotulo repetido "
                                 << "(linha " << line_count << ")." << endl;
                        }

                    } else if (command != "SECTION") {  // n eh transicao
                        if (!command.empty() && TD.count(command) > 0) {
                            cout << "Erro! Diretiva na secao errada "
                                 << "(linha " << line_count << ")." << endl;
                        } else if (!command.empty()) {
                            cout << "Erro! Diretiva invalida "
                                 << "(linha " << line_count << ")." << endl;
                        }
                    }
                } else if (!command.empty() && command != "SECTION") {  // sem rotulo e n eh transicao
                    cout << "Erro! Rotulo ausente "
                         << "(linha " << line_count << ")." << endl;
                }
            } else if (currentMajorState == MajorState::SecondPass) {  // generate obj code
                if (currentSection == Section::Text) {
                    if (!command.empty()) {
                        if (TI.count(command) > 0) {
                            Instruction instr = TI.at(command);
                            // vector<int> endr_param;

                            if (instr.qtd_operands) {
                                if (!param.empty()) {
                                    for (string &e : param) {
                                        if (TS.count(e) > 0) {
                                            instr.operands.push_back(TS[e]);
                                        } else {
                                            cout << "Erro! Rotulo ausente "
                                                 << "(linha " << line_count << ")." << endl;
                                        }
                                    }
                                    if (!instr.operands.empty())
                                        text_table.push_back(instr);
                                } else {
                                    cout << "Erro! Parametro invalido "
                                         << "(linha " << line_count << ")." << endl;
                                }
                            } else {    // instruction without operands
                                text_table.push_back(instr);
                            }
                        } else {
                            cout << "Erro! Instrucao invalida "
                                 << "(linha " << line_count << ")." << endl;
                        }
                    }
                }
            }

            // Update memory address
            if (TI.count(command) > 0) {
                end_count += TI.at(command).length;
            } else if (TD.count(command) > 0) {
                end_count += TD.at(command).length;
            }
        }

        if (currentMajorState == MajorState::FirstPass) {
            currentMajorState = MajorState::SecondPass;  // second pass
            currentSection = Section::None;

            in_file.clear();
            in_file.seekg(0, in_file.beg);

            line_count = 0;
            end_count = 0;

            if (!found_text) {
                cout << "Erro! secao TEXT faltando." << endl;
                cout << "Compilacao interrompida." << endl;
                stop = true;
            }
        } else {
            stop = true;
        }
    }

    // write on output file
    // _obj_one_line(out_file, text_table, data_table, TS);
    _obj_pretty(out_file, text_table, data_table, TS);
}
