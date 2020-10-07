#include "montador.h"

#include <string.h>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "tabela.h"

#define LINE_LENGTH 256
#define DELIMETERS_PRE " \t"  // not validate params ("," stays)
#define DELIMETERS_COMP " \t,"

using namespace std;

bool is_number(string &s) {
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

    // out_file << endl;
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

bool _get_instr(ifstream &in_file, RawInstruction &raw_instr, int &line) {
    // RawInstruction instr;
    bool complete_instr = false;
    bool is_comment = false;
    string label;
    string command;
    int command_line = 0;
    vector<string> operands;
    InstType instr_type;

    vector<string> tokens;
    string token;
    while (!complete_instr && !in_file.eof() && !in_file.bad()) {
        char c;
        in_file.get(c);

        if (c == ';') {  // skipping commments
            is_comment = true;
        }

        if (!is_comment) {
            if (c != '\n' && c != '\t' && c != ' ' && !in_file.eof()) {  // building token
                token.push_back(toupper(c));
            } else {                                         // formed token
                for (size_t i = 0; i < token.size(); i++) {  // checking token
                    if (!isalnum(token[i]) && token[i] != '_' && token[i] != ',' && token[i] != ':') {
                        cout << "Erro LEXICO! Token invalido \"" << token << "\" "
                             << "(linha " << line << ")." << endl;

                        label.clear();  // invalidate all previous informations
                        command.clear();
                        operands.clear();

                        token.clear();
                        break;
                    }
                }

                // valid token
                if (!token.empty()) {
                    if (token.find(":") != string::npos) {
                        if (label.empty()) {
                            label = token;
                            label.pop_back();
                        } else {
                            cout << "Erro SINTATICO! Mais de um rotulo por instrucao/diretiva "
                                 << "(linha " << line << ")." << endl;
                        }
                    }
                    // Need to know if is TI/TD to properly get operands (etc) of instr
                    // since '\n' isn't useful to mark the beginning of a new instr.
                    else if (TI.count(token) > 0) {
                        if (!command.empty()) {  // already has assigned command
                            cout << "Erro SINTATICO! Instrucao com a quantidade de operandos invalida "
                                 << "(linha " << command_line << ")." << endl;
                        }
                        command = token;
                        command_line = line;
                        instr_type = InstType::Type1;
                        operands.clear();
                    } else if (TD.count(token) > 0) {
                        if (!command.empty()) {  // already has assigned command
                            cout << "Erro SINTATICO! Diretiva com a quantidade de operandos invalida "
                                 << "(linha " << command_line << ")." << endl;
                        }
                        command = token;
                        command_line = line;
                        instr_type = InstType::Type2;
                        operands.clear();
                    } else if (!command.empty()) {            // pode ser parametros
                        if (instr_type == InstType::Type1) {  // instrucao
                            if (TI.at(command).qtd_operands > operands.size()) {
                                operands.push_back(token);
                            } else {
                                cout << "Erro SINTATICO! Instrucao com a quantidade de operandos invalida "
                                     << "(linha " << line << ")." << endl;
                            }
                        } else if (instr_type == InstType::Type2) {  // diretiva
                            if (TD.at(command).qtd_operands > operands.size()) {
                                operands.push_back(token);
                            } else {
                                cout << "Erro SINTATICO! Diretiva com a quantidade de operandos invalida "
                                     << "(linha " << line << ")." << endl;
                            }
                        }
                    } else {
                        cout << "Erro SINTATICO! Diretiva/Instrucao invalida \"" << token << "\" "
                             << "(linha " << line << ")." << endl;
                    }

                    token.clear();
                }
            }
        }

        // new line
        if (c == '\n') {
            is_comment = false;
            line++;
        }

        // Constructing instr
        if (!command.empty()) {
            if (instr_type == InstType::Type1) {
                if (operands.size() == TI.at(command).qtd_operands) {
                    complete_instr = true;

                    raw_instr.type = instr_type;
                    raw_instr.label = label;
                    raw_instr.instr_name = command;
                    raw_instr.operands = operands;
                    raw_instr.line = command_line;
                }
            } else if (instr_type == InstType::Type2) {
                if (operands.size() == TD.at(command).qtd_operands) {
                    complete_instr = true;

                    raw_instr.type = instr_type;
                    raw_instr.label = label;
                    raw_instr.instr_name = command;
                    raw_instr.operands = operands;
                    raw_instr.line = command_line;
                }
            }
        }
    }

    return complete_instr;
}

bool _get_instr_basic(ifstream &in_file, RawInstruction &raw_instr, int &line) {
    // RawInstruction instr;
    bool complete_instr = false;
    bool is_comment = false;
    string label;
    string command;
    int command_line = 0;
    vector<string> operands;
    InstType instr_type;

    vector<string> tokens;
    string token;
    while (!complete_instr && !in_file.eof() && !in_file.bad()) {
        char c;
        in_file.get(c);

        if (c == ';') {  // skipping commments
            is_comment = true;
        }

        if (!is_comment) {
            if (c != '\n' && c != '\t' && c != ' ' && !in_file.eof()) {  // building token
                token.push_back(toupper(c));
            } else {                                         // formed token
                for (size_t i = 0; i < token.size(); i++) {  // checking token
                    if (!isalnum(token[i]) && token[i] != '_' && token[i] != ',' && token[i] != ':') {
                        cout << "Erro LEXICO! Token invalido \"" << token << "\" "
                             << "(linha " << line << ")." << endl;

                        // label.clear();  // invalidate all previous informations
                        // command.clear();
                        // operands.clear();

                        token.clear();
                        break;
                    }
                }

                // valid token
                if (!token.empty()) {
                    if (token.find(":") != string::npos) {
                        label = token;
                        label.pop_back();
                        command.clear();
                        operands.clear();
                        // if (!isdigit(token[0]) && token.size() <= 50) {
                        // } else {
                        //     cout << "Erro SINTATICO! Rotulo invalido \"" << token << "\" "
                        //          << "(linha " << line << ")." << endl;
                        // }
                    }
                    // Only checks if a given command exists
                    else if (TI.count(token) > 0) {
                        command = token;
                        command_line = line;
                        instr_type = InstType::Type1;
                        operands.clear();
                    } else if (TD.count(token) > 0) {
                        command = token;
                        command_line = line;
                        instr_type = InstType::Type2;
                        operands.clear();
                    } else if (!command.empty()) {  // pode ser parametros
                        operands.push_back(token);
                    } else {
                        cout << "Erro SINTATICO! Diretiva/Instrucao invalida \"" << token << "\" "
                             << "(linha " << line << ")." << endl;
                    }

                    token.clear();
                }
            }
        }

        // new line
        if (c == '\n') {
            is_comment = false;
            line++;
        }

        // Constructing instr
        if (!command.empty() && (c == '\n' || in_file.eof())) {
            complete_instr = true;

            raw_instr.type = instr_type;
            raw_instr.label = label;
            raw_instr.instr_name = command;
            raw_instr.operands = operands;
            raw_instr.line = command_line;
        }
    }

    return complete_instr;
}

void pre_process(ifstream &in_file, ofstream &out_file) {
    RawInstruction raw_instr;
    int line_count = 1;

    map<string, int> equ_table;

    enum class MajorState { Equ,
                            Text,
                            Data,
                            PassThrough };
    MajorState currentMajorState = MajorState::Equ;
    // MajorState previousMajorState = currentMajorState;

    enum class TextState { Get,
                           Pass };
    TextState currentTextState = TextState::Get;

    while (!in_file.eof() && !in_file.bad()) {
        raw_instr.clear();
        if (_get_instr(in_file, raw_instr, line_count)) {
            // check state
            if (raw_instr.instr_name == "SECTION") {
                if (raw_instr.operands.front() == "TEXT") {
                    currentMajorState = MajorState::Text;
                } else if (raw_instr.operands.front() == "DATA") {
                    currentMajorState = MajorState::Data;
                    // add \n on state transition
                    out_file << endl;
                } else {
                    cout << "Erro SINTATICO! Secao invalida \"" << raw_instr.operands.front() << "\" "
                         << "(linha " << line_count << ")." << endl;
                }
            }

            if (currentMajorState == MajorState::Equ) {
                if (raw_instr.instr_name == "EQU") {
                    if (equ_table.count(raw_instr.label) == 0) {
                        if (is_number(raw_instr.operands.front())) {
                            equ_table[raw_instr.label] = stoi(raw_instr.operands.front());
                        } else {
                            cout << "Erro SINTATICO! Tipo de operando invalido "
                                 << "(linha " << line_count << ")." << endl;
                        }
                    } else {
                        cout << "Erro SEMANTICO! Rotulo repetido \"" << raw_instr.label << "\" "
                             << "(linha " << line_count << ")." << endl;
                    }
                }
            } else if (currentMajorState == MajorState::Text || currentMajorState == MajorState::Data) {
                if (raw_instr.instr_name == "IF") {
                    if (equ_table.count(raw_instr.operands.front()) > 0) {  // check param(label) on equ_table
                        if (equ_table.at(raw_instr.operands.front()) != 1) {
                            currentTextState = TextState::Pass;
                        }
                    } else {  // didnt find correspondent label
                        currentTextState = TextState::Pass;
                        cout << "Erro SEMANTICO! Declaracao ausente "
                             << "(linha " << line_count << ")." << endl;
                    }
                } else if (currentTextState == TextState::Get) {
                    if (!raw_instr.label.empty()) {
                        out_file << raw_instr.label << ": " << raw_instr.instr_name << ' ';
                    } else {
                        out_file << raw_instr.instr_name << ' ';
                    }
                    for (auto &e : raw_instr.operands) out_file << e << ' ';
                    out_file << '\n';
                } else if (currentTextState == TextState::Pass) {
                    currentTextState = TextState::Get;
                }
            }
        }
    }
}

void pre_process_basic(ifstream &in_file, ofstream &out_file) {
    RawInstruction raw_instr;
    int line_count = 1;

    map<string, int> equ_table;

    enum class MajorState { Equ,
                            Text,
                            Data,
                            PassThrough };
    MajorState currentMajorState = MajorState::Equ;
    // MajorState previousMajorState = currentMajorState;

    enum class TextState { Get,
                           Pass };
    TextState currentTextState = TextState::Get;

    while (!in_file.eof() && !in_file.bad()) {
        raw_instr.clear();
        if (_get_instr_basic(in_file, raw_instr, line_count)) {
            // check state
            if (raw_instr.instr_name == "SECTION") {
                if (raw_instr.operands.front() == "TEXT") {
                    currentMajorState = MajorState::Text;
                } else if (raw_instr.operands.front() == "DATA") {
                    currentMajorState = MajorState::Data;
                    // add \n on state transition
                    out_file << endl;
                }
            }

            if (currentMajorState == MajorState::Equ) {
                if (raw_instr.instr_name == "EQU") {
                    if (equ_table.count(raw_instr.label) == 0) {
                        if (is_number(raw_instr.operands.front())) {
                            equ_table[raw_instr.label] = stoi(raw_instr.operands.front());
                        } else {
                            cout << "Erro SINTATICO! Tipo de operando invalido \"" << raw_instr.operands.front() << "\" "
                                 << "(linha " << raw_instr.line << ")." << endl;
                        }
                    } else {
                        cout << "Erro SEMANTICO! Rotulo repetido \"" << raw_instr.label << "\" "
                             << "(linha " << raw_instr.line << ")." << endl;
                    }
                }
            } else if (currentMajorState == MajorState::Text || currentMajorState == MajorState::Data) {
                if (raw_instr.instr_name == "IF") {
                    if (equ_table.count(raw_instr.operands.front()) > 0) {  // check param(label) on equ_table
                        if (equ_table.at(raw_instr.operands.front()) != 1) {
                            currentTextState = TextState::Pass;
                        }
                    } else {  // didnt find correspondent label
                        currentTextState = TextState::Pass;
                        cout << "Erro SEMANTICO! Rotulo ausente \"" << raw_instr.operands.front() << "\" "
                             << "(linha " << raw_instr.line << ")." << endl;
                    }
                } else if (currentTextState == TextState::Get) {
                    if (!raw_instr.label.empty()) {
                        out_file << raw_instr.label << ": " << raw_instr.instr_name << ' ';
                    } else {
                        out_file << raw_instr.instr_name << ' ';
                    }
                    for (auto &e : raw_instr.operands) out_file << e << ' ';
                    out_file << '\n';
                } else if (currentTextState == TextState::Pass) {
                    currentTextState = TextState::Get;
                }
            }
        }
    }
}

bool compile(ifstream &in_file, ofstream &out_file, int mode) {
    enum class MajorState { FirstPass,
                            SecondPass,
                            PassThrough };
    MajorState currentMajorState = MajorState::FirstPass;
    MajorState previousMajorState = currentMajorState;

    enum class Section { None,
                         Text,
                         Data };
    Section currentSection = Section::None;

    bool stop = false;
    bool fatal_error = false;
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
                pch = strtok(line, DELIMETERS_COMP);
                while (pch != NULL) {
                    token.push_back(pch);

                    pch = strtok(NULL, DELIMETERS_COMP);
                }
            }

            // specifing
            string label;
            string command;
            vector<string> param;

            for (string &e : token) {
                if (e.find(":") != string::npos) {  // label
                    if (label.empty()) {
                        if (e.size() < 50 && isalpha(e[0])) {
                            label = e;
                            label.pop_back();
                        } else if (currentMajorState == MajorState::SecondPass) {
                            cout << "Erro LEXICO! Rotulo invalido \"" << e << "\" "
                                 << "(linha " << line_count << ")." << endl;
                        }
                    } else if (currentMajorState == MajorState::SecondPass) {  // show errors one time only
                        cout << "Erro SINTATICO! Mais de um rotulo na mesma linha "
                             << "(linha " << line_count << ")." << endl;
                    }
                } else if (TI.count(e) > 0) {  // instruction
                    if (command.empty()) {
                        command = e;
                    } else if (currentMajorState == MajorState::SecondPass) {
                        cout << "Erro SINTATICO! Instrucao com tipo de operando invalido \"" << e << "\" "
                             << "(linha " << line_count << ")." << endl;
                    }
                } else if (TD.count(e) > 0) {  // directive
                    if (command.empty()) {
                        command = e;
                    } else if (currentMajorState == MajorState::SecondPass) {
                        cout << "Erro SINTATICO! Diretiva com tipo de operando invalido \"" << e << "\" "
                             << "(linha " << line_count << ")." << endl;
                    }
                } else {  // parameters isn't checked yet
                    param.push_back(e);
                }
            }

            // Empty line (\n)
            if (token.empty()) {
                currentMajorState = MajorState::PassThrough;
            }

            // Check section
            if (!command.empty() && token[0] == "SECTION") {
                if (param.size() == TD.at("SECTION").qtd_operands) {
                    if (param.front() == "TEXT") {
                        found_text = true;
                        currentSection = Section::Text;
                    } else if (param.front() == "DATA") {
                        currentSection = Section::Data;
                    } else if (currentMajorState == MajorState::SecondPass) {
                        cout << "Erro SINTATICO! Secao invalida \"" << param.front() << "\" "
                             << "(linha " << line_count << ")." << endl;
                    }
                } else if (currentMajorState == MajorState::SecondPass) {  // show error only on second pass
                    cout << "Erro SINTATICO! Diretiva  \"" << command << "\" com quantidade de operandos invalida "
                         << "(linha " << line_count << ")." << endl;
                }

                currentMajorState = MajorState::PassThrough;
            }

            if (currentMajorState == MajorState::FirstPass) {  // make TS, check labels and directives
                if (currentSection == Section::Text) {
                    if (!label.empty()) {
                        if (TS.count(label) == 0) {
                            TS[label] = end_count;
                        } else {
                            cout << "Erro SEMANTICO! Rotulo repetido \"" << label << "\" "
                                 << "(linha " << line_count << ")." << endl;
                        }
                    }
                } else if (currentSection == Section::Data) {
                    if (!label.empty()) {
                        if (TS.count(label) == 0) {
                            if (command == "SPACE") {
                                TS[label] = end_count;
                                data_table[end_count] = 0;

                                if (!param.empty()) {  // SPACE n tem param
                                    cout << "Erro SINTATICO! Diretiva \"" << command << "\" com a quantidade de operandos invalida "
                                         << "(linha " << line_count << ")." << endl;
                                }
                            } else if (command == "CONST") {
                                TS[label] = end_count;

                                if (param.size() == TD.at(command).qtd_operands) {  // check qtd de param
                                    if (is_number(param.front())) {                 // param eh numero?
                                        data_table[end_count] = stoi(param.front());
                                    } else {
                                        cout << "Erro SINTATICO! Diretiva \"" << command << "\"  com o tipo de operandos invalido "
                                             << "(linha " << line_count << ")." << endl;
                                    }
                                } else {
                                    cout << "Erro SINTATICO! Diretiva \"" << command << "\" com a quantidade de operandos invalida "
                                         << "(linha " << line_count << ")." << endl;
                                }
                            } else {
                                cout << "Erro SINTATICO! Diretiva invalida \"" << command << "\" "
                                     << "(linha " << line_count << ")." << endl;
                            }
                        } else {
                            cout << "Erro SEMANTICO! Rotulo repetido \"" << label << "\" "
                                 << "(linha " << line_count << ")." << endl;
                        }

                    } else if (command != "SECTION") {  // n eh transicao
                        cout << "Erro SINTATICO! Rotulo ausente "
                             << "(linha " << line_count << ")." << endl;
                    }
                } else if (!command.empty() && command != "SECTION") {  // sem rotulo e n eh transicao
                    cout << "Erro SEMANTICO! Diretiva/Instrucao na secao errada \"" << command << "\" "
                         << "(linha " << line_count << ")." << endl;
                }
            } else if (currentMajorState == MajorState::SecondPass) {  // generate obj code, check instr's params, check labels on TS
                if (currentSection == Section::Text) {
                    if (!command.empty()) {
                        if (TI.count(command) > 0) {
                            Instruction instr = TI.at(command);

                            if (instr.qtd_operands == param.size()) {
                                if (instr.qtd_operands > 0) {  // has param
                                    bool valid_param = true;
                                    for (size_t i = 0; i < instr.qtd_operands; i++) {  // check param on TS
                                        if (TS.count(param[i]) > 0) {
                                            instr.operands.push_back(TS[param[i]]);
                                        } else {
                                            if (is_number(param[i])) {  // number
                                                cout << "Erro SINTATICO! Parametro invalido \"" << param[i] << "\" "
                                                     << "(linha " << line_count << ")." << endl;
                                            } else {  // name
                                                cout << "Erro SEMANTICO! Rotulo ausente \"" << param[i] << "\" "
                                                     << "(linha " << line_count << ")." << endl;
                                            }
                                            valid_param = false;
                                        }
                                    }
                                    if (valid_param)
                                        text_table.push_back(instr);
                                } else {  // instr without param
                                    text_table.push_back(instr);
                                }
                            } else {
                                cout << "Erro SINTATICO! Instrucao \"" << command << "\" com a quantidade de operandos incorreta "
                                     << "(linha " << line_count << ")." << endl;
                            }
                        } else if (TD.count(command) > 0) {  // (occurs if command is a directive)
                            cout << "Erro SEMANTICO! Diretiva na secao errada \"" << command << "\" "
                                 << "(linha " << line_count << ")." << endl;
                        } else {
                            cout << "Erro SINTATICO! Instrucao invalida \"" << command << "\" "
                                 << "(linha " << line_count << ")." << endl;
                        }
                    } else {
                        cout << "Erro SINTATICO! Declaracao ausente "
                             << "(linha " << line_count << ")." << endl;
                    }
                }
            }

            // Update memory address
            if (TI.count(command) > 0) {
                end_count += TI.at(command).length;
            } else if (TD.count(command) > 0) {
                end_count += TD.at(command).length;
            }

            // Recovering state
            if (currentMajorState == MajorState::PassThrough) {
                currentMajorState = previousMajorState;
            }
        }

        if (currentMajorState == MajorState::FirstPass) {
            currentMajorState = MajorState::SecondPass;  // second pass
            previousMajorState = currentMajorState;

            currentSection = Section::None;

            in_file.clear();
            in_file.seekg(0, in_file.beg);

            line_count = 0;
            end_count = 0;

            if (!found_text) {
                cout << "Erro SEMANTICO! Secao TEXT faltando." << endl;
                cout << "> Compilacao interrompida." << endl;

                fatal_error = true;
                stop = true;
            }
        } else {
            stop = true;
        }
    }

    // write on output file
    if (!fatal_error) {
        if (mode == 1) {
            _obj_pretty(out_file, text_table, data_table, TS);
        } else {
            _obj_one_line(out_file, text_table, data_table, TS);
        }
    }

    return !fatal_error;
}
