// #include <string.h>

#include <fstream>
#include <iostream>
// #include <iomanip>
#include <string>
#include <vector>
#include <set>

#include "tabela.h"
#include "montador.h"

const set<string> Parameters {
    {"-p"},
    {"-o"},
    {"-op"}
};

using namespace std;

int get_arguments(int argc, char **argv, string &option, string &sf) {
    int ret = 0;

    if (argc == 3) {  // flag and source file name
        for (int i = 1; i < argc; i++) {
            if (option.empty() && Parameters.count((string)argv[i]) > 0) {
                option = argv[i];
            } else if (sf.empty()) {
                sf = argv[i];
            }
        }

        if (!option.empty()) {
            string::size_type found;

            if (option == "-p") {
                found = sf.find(".asm");
            } else if (option == "-o" || option == "-op") {
                found = sf.find(".PRE");
            }

            if (found == string::npos) {
                cout << "Erro! Nome do arquivo invalido." << endl;
                cout << "> -p: <nome>.asm" << endl;
                cout << "> -o: <nome>.PRE" << endl;

                ret = 1;
            } else {
                string aux = sf;
                aux.erase(found, aux.length());
                if (aux.empty()) {
                    cout << "Erro! Nome do arquivo invalido." << endl;
                    cout << "> \"" << sf << "\"" << endl;

                    ret = 1;
                }
            }
        } else {
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

int main(int argc, char **argv) {
    string option;
    string sf;

    if (get_arguments(argc, argv, option, sf)) {
        return 0;
    }

    // key: rótulo, content: end. memória
    map<string, int> symbol_table;
    ifstream in_file(sf);
    ofstream out_file;
    if (!in_file.is_open()) {
        cout << "Erro! Arquivo \"" << sf << "\" nao encontrado." << endl;
        return 0;
    }

    if (option == "-p") {
        string::size_type found = sf.find(".asm");
        string out_file_name = sf.erase(found, sf.length());
        out_file_name += ".PRE";

        out_file.open(out_file_name);
        if (!out_file.is_open()) {
            cout << "Erro! Falha ao criar arquivo: " << out_file_name << endl;
            return 0;
        }
        pre_process2(in_file, out_file);
        cout << "> Gerado arquivo \"" << out_file_name << "\"" << endl;
    } else if (option == "-o" || option == "-op") {
        string::size_type found = sf.find(".PRE");
        string out_file_name = sf.erase(found, sf.length());
        out_file_name += ".OBJ";

        out_file.open(out_file_name);
        if (!out_file.is_open()) {
            cout << "Erro! Falha ao criar arquivo: " << out_file_name << endl;
            return 0;
        }
        if (option == "-op") {
            compile(in_file, out_file, 1);
        } else {
            compile(in_file, out_file);
        }
        cout << "> Gerado arquivo \"" << out_file_name << "\"" << endl;
    }

    in_file.close();
    out_file.close();

    return 0;
}
