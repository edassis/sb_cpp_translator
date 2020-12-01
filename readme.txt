Aluno:
- Nome: Eduardo Ferreira de Assis
- Matricula: 17/0102289


Informações:
- O que foi implementado:
    Tradutor do assembly inventado para assembly x86. Ele faz
    pré-processamento (temp.PRE), onde checa erros.

    -> "./tradutor <nome>.asm" Gera o arquivo <nome>.s traduzido. 

    Foi implementado a biblioteca "inout.asm" de procedimentos em
    Assembly x86 para fazer as requisições de I/O. Esse arquivo precisa
    estar junto com o arquivo traduzido na hora da compilação.

    -> Compilar o assembly:

        $ ./tradutor <file>.asm
        $ nasm -f elf32 <file>.s
        $ ld -m elf_i386 <file>.o


- Instruções para compilar o programa:
    Foi criado um Makefile para facilitar o processo de compilação.
    Bastando executar "make" para gerar todos os arquivos objetos em "obj/"
    e o executável "tradutor".

    -> Compilação com o Makefile:
        $ make
    
    -> Compilação manual:
        $ g++ -std=c++11 -Wall -g -I header/ src/main.cpp src/montador.cpp -o tradutor

    -> Executar:
        $ ./tradutor <nome>.asm


Versão do software:
    -> g++ (Ubuntu 9.3.0-10ubuntu2) 9.3.0
    -> NASM version 2.14.02
    -> GNU ld (GNU Binutils for Ubuntu) 2.34