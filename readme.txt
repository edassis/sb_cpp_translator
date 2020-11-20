Aluno:
- Nome: Eduardo Ferreira de Assis
- Matricula: 17/0102289


Informações:
- O que foi implementado:
    Implementado montador com CHECAGEM DE ERROS conforme especificado na
    ementa do trabalho. Rótulos/Instruções/Diretivas com erro são ignoradas.

    "./montador -p <nome>.asm" Gera o arquivo <nome>.PRE sem comentários, 
    espaços/tabulações desnecessários. 
    
        Nessa etapa, o montador checa se dado comando é uma instrução/diretiva
        válida e trata a diretiva EQU. Confere a construção do token (checagem léxica).

    "./montador -o <nome>.PRE" Gera o arquivo <nome>.OBJ contendo o binário.

        Nessa etapa, o montador procura por todos os tipos de erros. Sempre que
        possível gera o binário.


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


- Compilar para linux:
	$ nasm -g -f elf -o hello.o hello.asm
	$ ld -m elf_i386 -o hello hello.o

	$ nasm -f win32 ass.asm -o ass.obj
	$ ld -mi386pe -o ass.exe ass.obj


- Versão do compilador:
    -> g++ (Ubuntu 9.3.0-10ubuntu2) 9.3.0


gdb:
layout regs
objdump -S assets/assembly/IA32/inout.o > fon.txt    
objdump -D assets/assembly/IA32/inout.o > fon.txt
readelf -x .data assets/assembly/IA32/inout.o > fon.txt  
readelf -a .data assets/assembly/IA32/inout.o > fon.txt  