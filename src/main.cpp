#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <vector>
#include "tabela.h"

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

void pre_process(FILE* in_file, FILE* out_file) {
    enum class MajorState {Equ, Text, Data, None};
    MajorState cur_major_state = MajorState::Equ;
    bool change_section = false;
    bool is_comment = false;

    enum class EquState {Label, Comand, Number};
    EquState cur_equ_state = EquState::Label;
    
    string token;
    
    string rotulo;
    string comando;
    int number;
    map<string, int> tabela_equ;

    enum class TextState {Check, Get, Skip};
    TextState cur_text_state = TextState::Get;
    bool is_if = false;
    string equ_label = "";

    int line_count = 1;
    
    char c;
    do {
        c = fgetc(in_file);
        
        if (isalpha(c) || c == ':') {  // letras
            if (c >= 'a') {
                c -= 'a' - 'A';
                token += c;
            }
            else {
                token += c;
            }
        }
        else if (isdigit(c)) {   // numeros
            token += c;
        }
        else if (c == ';') {
            is_comment = true;
        }
        // else if (c == ';') {    // comments
        //     while(c =fgetc(in_file), c != '\n' && c != EOF);
        //     line_count++;
        //     c = ' ';    // Force token analysis
        // }
        // else {
        //     cout << "Erro! Token invalido! (linha " << line_count << ")." << endl;
        // }

        if ((isspace(c) || iscntrl(c) || is_comment) || c == EOF) {
            if (!token.empty()) {
                if (token == "SECTION") {
                    cout << token << ' ';
                    change_section = true;
                    cur_major_state = MajorState::None;
                }
                if (change_section) {
                    if (token == "TEXT") {
                        cout << token << endl;
                        cur_major_state = MajorState::Text;
                        change_section = false;
                        token.clear();
                        continue;
                    }   
                    else if (token == "DATA") {
                        cout << token << endl;
                        cur_major_state = MajorState::Data;
                        change_section = false;
                        token.clear();
                        continue;
                    }
                // else {
                //     cout << "Erro! Diretiva invalida (linha " << line_count << ")." << endl;
                // }
                }
            
                if (cur_major_state == MajorState::Equ) {
                    if (cur_equ_state == EquState::Label) {
                        if (token.back() == ':') {  // rotulo
                            token.pop_back();
                            rotulo = token;

                            cur_equ_state = EquState::Comand;
                        }
                    }
                    else if (cur_equ_state == EquState::Comand) {
                        if (token == "EQU") {
                            comando = "EQU";
                            cur_equ_state = EquState::Number;
                        }
                    }
                    else if (cur_equ_state == EquState::Number) {
                        if (token.front() >= '0' && token.front() <= '9') {     // numero
                            number = stoi(token);
                        }

                        map<string, int>::iterator it = tabela_equ.find(rotulo);
                        if (it != tabela_equ.end()) {    // ja tem na tabela
                            cout << "ERRO! tem na tabela" << endl;
                            // gera erro
                        }
                        else {  // add na TS
                            // cout << rotulo << " " << comando << " " << number << endl;
                            tabela_equ.insert({rotulo, number});
                        }

                        cur_equ_state = EquState::Label;
                    }
                }
            }
            if (cur_major_state == MajorState::Text) {
                if (cur_text_state == TextState::Get) {
                    if (token == "IF") cur_text_state = TextState::Check;
                    else if (!token.empty() && !is_comment)
                        cout << token << " ";
                    else if (is_comment || c == '\n')
                        cout << endl;
                }
                else if (cur_text_state == TextState::Check) {
                    if (tabela_equ.count(token) > 0) {
                        int valor = tabela_equ[token];
                        if (valor == 1) {
                            cur_text_state = TextState::Get;
                        }
                        else {
                            cur_text_state = TextState::Skip;
                        }
                    }
                    else {
                        cout << "Erro! Declaracoes e rotulos ausentes (linha " << line_count << ")." << endl;
                    }
                    //is_if = false;
                }
                else if (cur_text_state == TextState::Skip) {
                    if (c != '\n' && c != EOF) {    // ainda n eh final de linha
                        while(c = fgetc(in_file), c != '\n' && c != EOF);
                        line_count++;
                    }
                    while(c = fgetc(in_file), c != '\n' && c != EOF);
                    line_count++;

                    cur_text_state = TextState::Get;
                }
            }
            else if (cur_major_state == MajorState::Data) {
                if (!is_comment)
                    cout << token << ' ';
                
                if (is_comment || c == '\n' || c == EOF)
                    cout << '\n';
            }
            
            token.clear();
        }

        if (is_comment) {     // jump comments
            while(c = fgetc(in_file), c != '\n' && c != EOF);
            is_comment = false;
        }
        if (c == '\n') {
            line_count++;
            // cout << endl;
        }
    } while (c != EOF);


    for(auto e:tabela_equ) {
        cout << e.first << " " << e.second << endl;
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
    FILE *in_file = fopen(sf.c_str(), "r");
    
    if (in_file == NULL) {
        cout << "Erro! Arquivo \"" << sf << "\" nao encontrado." << endl;
        return 0;
    }

    if (option == "-p") {
        string::size_type found = sf.find(".asm");
        string out_file_name = sf.erase(found, sf.length());
        out_file_name += ".PRE";

        FILE *out_file = fopen(out_file_name.c_str(), "w");
        if (out_file == NULL) {
            cout << "Erro ao criar arquivo: " << out_file_name << endl;
            return 0;
        }
        pre_process(in_file, out_file);
        // cout << "Pre-processar" << endl;
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