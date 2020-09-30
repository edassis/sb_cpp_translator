#include <iostream>
#include <vector>
#include "../header/tabela.h"

using namespace std;


int arg(int argc, char **argv, string& option, string& sf) {
    int ret = 0;
    
    if (argc >= 3) {    // flag and source file name
        for (int i = 1; i < argc; i++) {
            if (option.empty() && ( ((string)argv[i]).compare("-p") == 0 || ((string)argv[i]).compare("-o") == 0) ) {
                option = argv[i];
            }
            else if (sf.empty()) {
                sf = argv[i];
            }
        }

        string::size_type found = sf.find(".asm");
        if (option.empty() || found == string::npos) ret = 1;
        else {
            sf.erase(found, sf.length());
            if (sf.empty()) ret = 1;
        }
    } else {
        ret = 1;
    }
    
    cout << option << " " << sf << endl;
    
    return ret;
}

int main(int argc, char **argv) {
    string option;
    string sf;

    if (arg(argc, argv, option, sf)) {
        cout << "Erro! Passagem invalida de parametros." << endl;
        cout << "> Informe uma opcao: -p, -o" << endl;
        cout << "> Informe o nome do arquivo fonte (i.e., bin.asm)" << endl;
        return 0;
    }

    if (option == "-o") {
        cout << "Montar" << endl;
    }
    else if (option == "-p") {
        cout << "Pre-processar" << endl;
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