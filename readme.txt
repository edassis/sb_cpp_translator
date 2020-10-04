Aluno:
- Nome: Eduardo Ferreira de Assis
- Matricula: 17/0102289


Informações:
- O que foi implementado:
    Implementado montador com CHECAGEM DE ERROS conforme especificado na
    ementa do trabalho.

    "./montador -p <nome>.asm" Gera o arquivo <nome>.PRE sem comentários, 
    espaços/tabulações desnecessários. 
    
        Nessa etapa, o montador já faz a checagem
        de erros LÉXICOS e alguns erros SINTÁTICOS referentes a construção
        dos tokens/instrução/diretiva.

    "./montador -o <nome>.PRE" Gera o arquivo <nome>.OBJ contendo o binário.

        Nessa etapa, o montador procura por erros de SINTAXE e erros
        SEMÂNTICOS.


- Instruções para compilar:
    Foi criado um Makefile para facilitar processo de compilação.
    Bastando executar "make" para gerar todos os arquivos objetos em "obj/"
    e o executável "montador".

    -> Compilação com o Makefile:
        $ make
    
    -> Compilação manual:
        $ g++ -std=c++11 -Wall -g -I header/ src/main.cpp src/montador.cpp -o montador

    -> Executar:
        $ ./montador


- Versão do compilador:
    -> g++ (Ubuntu 9.3.0-10ubuntu2) 9.3.0