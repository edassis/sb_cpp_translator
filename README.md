# SB-Tradutor

A C++ translator for the assembly invented in class.

## Pre-Process

Remove unnecessary spaces, tabs, and do basic code formatting (`.PRE` file).
Do some checks of tokens validity and instruction/directive formation.
    
    ./montador -p <name>.asm


## Compile

Generate binary code (`.OBJ` file).
Checks function/diretives semantically params validity.
    
    ./montador -o <name>.PRE



![Tabela de comandos][1]


[1]: images/OPCODE-Table.PNG