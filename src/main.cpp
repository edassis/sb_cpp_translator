#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <vector>
#include "tabela.h"

#define LINE_LENGTH 256
#define DELIMETERS " "

using namespace std;

int get_arguments(int argc, char **argv, string& option, string& sf) {
    int ret = 0;
    
    if (argc == 3) {    // flag and source file name
        for (int i = 1; i < argc; i++) {
            if (option.empty() && ( ((string)argv[i]).compare("-p") == 0 || ((string)argv[i]).compare("-o") == 0) ) {
                option = argv[i];
            }
            else if (sf.empty()) {
                sf = argv[i];
            }
        }

        if (!option.empty()) {
            string::size_type found;
            
            if (option.compare("-p") == 0) {
                found = sf.find(".asm");
            }
            else if (option.compare("-o") == 0) {
                found = sf.find(".PRE");
            }

            if (found == string::npos) {
                cout << "Erro! Nome do arquivo invalido." << endl;
                cout << "> -p: <nome>.asm" << endl;
                cout << "> -o: <nome>.PRE" << endl;
                
                ret = 1;
            }
            else {
                string aux = sf;
                aux.erase(found, aux.length());
                if (aux.empty()) {
                    cout << "Erro! Nome do arquivo invalido." << endl;
                    cout << "> \"" << sf << "\"" << endl;
                    
                    ret = 1;
                }
            }
        }
        else {
            cout << "Erro! Passagem invalida de parametros." << endl;
            cout << "> Informe uma opcao: -p, -o" << endl;
            cout << "> Informe o nome do arquivo fonte (i.e., bin.asm)" << endl;
            
            ret = 1;
        }
    } else {
        cout << "Erro! Passagem invalida de parametros." << endl;
        cout << "> Informe uma opcao: -p, -o" << endl;
        cout << "> Informe o nome do arquivo fonte (i.e., bin.asm)" << endl;

        ret = 1;
    }
    
    // cout << option << " " << sf << endl;
    
    return ret;
}


void pre_process(ifstream &in_file, ofstream &out_file) {
    char line[LINE_LENGTH];
    vector<string> token;
    int line_count = 0;

    enum class MajorState {Equ, Text, Data, PassThrough};
    MajorState currentMajorState = MajorState::Equ;
    MajorState previousMajorState = currentMajorState;
    
    enum class TextState {Get, Skip};
    TextState currentTextState = TextState::Get;

    map<string, int> equ_table;
    string output_line;

    while(!in_file.eof() && !in_file.bad()) {
        if (currentMajorState == MajorState::PassThrough) {
            currentMajorState = previousMajorState;
        }
        output_line.clear();

        // Get line
        in_file.getline(line, LINE_LENGTH);
        line_count++;

        // cout << line_count << ": " << line << endl;
        // if (line[0] == '\0') {
        //     currentMajorState = MajorState::PassThrough;
        //     output_line = '\n';
        // }

        for (int i = 0; i < LINE_LENGTH; i++)  {
            line[i] = toupper(line[i]);     // Uppercase
            
            if (line[i] == ';') {       // Removing coments
                line[i] = '\0';
                break;
            }
        }

        // Tokenize
        token.clear();
        char *pch;
        // if (token.empty())
        //     pch = strtok(line, DELIMETERS);
        // else
        //     pch = strtok(NULL, DELIMETERS);
        if (line[0] != '\0') {
            pch = strtok(line, DELIMETERS);
            if (pch == NULL) {
                cout << "Erro! Nao foi possivel formar o token " << "(linha " << line_count << ")." << endl;
                currentMajorState = MajorState::PassThrough;
            }
            while (pch != NULL) {
                bool valid_token = true;
                for (size_t i = 0; i < strlen(pch); i++) {
                    // Check characters
                    if(!isalnum(pch[i]) && pch[i] != ':' && pch[i] != '_' && pch[i] != ',') {
                        cout << "Erro! Nao foi possivel formar o token " << "(linha " << line_count << ")." << endl;
                        valid_token = false;
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
            }
            else if (token[1] == "DATA" && token.size() == 2) {
                currentMajorState = MajorState::Data;
                previousMajorState = currentMajorState;
                output_line = "SECTION DATA\n";
            }
            else {
                cout << "Erro! Secao invalida " << "(linha " << line_count << ")." << endl;
            }

            currentMajorState = MajorState::PassThrough;
        }

        // States
        if (currentMajorState == MajorState::Equ) {
            if (!token.empty() && token.front().find(":") != string::npos) {
                string label = token.front();
                label.pop_back();

                if (token[1] == "EQU" ) {       // Check command
                    bool is_number = true;      // Check number
                    for (char &c:token[2]) {
                        if (!isdigit(c)) {
                            is_number = false;
                            break;
                        }
                    }
                    if(is_number) {
                        int number = stoi(token[2]);

                        if (equ_table.count(label) > 0) {
                            cout << "Erro! Rotulo repetido " << "(linha " << line_count << ")." << endl;
                        }
                        else {
                            // cout << label << ' ' << number << endl;
                            equ_table[label] = number;
                        }
                    }
                }
            }
        }
        else if (currentMajorState == MajorState::Text) {
            if (token[0] == "IF") {
                if (equ_table.count(token[1]) > 0) {    // Search equ label
                    if (equ_table[token[1]] == 1) {
                        currentTextState = TextState::Get;
                    }
                    else {
                        currentTextState = TextState::Skip;
                    }
                }
            }
            else if (currentTextState == TextState::Get) {
                for (size_t i = 0; i < token.size(); i++) {
                    output_line += token[i];
                    if (i+1 < token.size()) output_line += ' ';
                }
                output_line += '\n';
            }
            else if (currentTextState == TextState::Skip) {
                output_line.clear();
                currentTextState = TextState::Get;
            }
        }
        else if (currentMajorState == MajorState::Data) {
            for (size_t i = 0; i < token.size(); i++) {
                output_line += token[i];
                if (i+1 < token.size()) output_line += ' ';
            }
            output_line += '\n';
        }
        
        if (!output_line.empty())
            out_file << output_line;
    }
}


int main(int argc, char **argv) {
    string option;
    string sf;

    if (get_arguments(argc, argv, option, sf)) {
        return 0;
    }
    
    // key: rótulo, content: end. memória
    map<string, int> symbol_table;
    ifstream in_file(sf);
    if (!in_file.is_open()) {
        cout << "Erro! Arquivo \"" << sf << "\" nao encontrado." << endl;
        return 0;
    }

    if (option == "-p") {
        string::size_type found = sf.find(".asm");
        string out_file_name = sf.erase(found, sf.length());
        out_file_name += ".PRE";

        ofstream out_file(out_file_name);
        if (!out_file.is_open()) {
            cout << "Erro ao criar arquivo: " << out_file_name << endl;
            return 0;
        }
        pre_process(in_file, out_file);
        in_file.close();
        out_file.close();
    }
    else if (option == "-o") {
        cout << "Montar" << endl;
    }

    return 0;
}

// Pegar parametros do terminal
// Argumentos:
// -p : pre-processa, saida <nome>.PRE
// -o : monta, saida <nome>.OBJ (requere arquivo .PRE)

// O comando não termina ao quebrar a linha



//OBS
// Assumir que o EQU sempre vai vir no inicio do programa e fora das se¸c˜oes de
// Texto e Dados. Lembrar que pode ter EQU sem IF, mas assumir que IF sempre precisa
// de uma declara¸c˜ao de EQU anteior