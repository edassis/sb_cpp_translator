#include <iostream>

using namespace std;

int main(int argc, char **argv) {
    for (int i = 0; i < argc; i++) {
        cout << argv[i] << endl;
    }
    
    return 0;
}

// Pegar parametros do terminal
// Argumentos:
// -p : pre-processa, saida <nome>.PRE
// -o : pre-processa e monta, saida <nome.OBJ

// O comando não termina ao quebrar a linha



//OBS
// Assumir que o EQU sempre vai vir no inicio do programa e fora das se¸c˜oes de
// Texto e Dados. Lembrar que pode ter EQU sem IF, mas assumir que IF sempre precisa
// de uma declara¸c˜ao de EQU anteior