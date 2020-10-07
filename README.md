# SB-Tradutor

A C++ translator for the assembly invented in class.

## Pre-Process

Remove unnecessary spaces, tabs, comments, and do basic code formatting (`.PRE` file).
Checks whether a given command is valid.
    
    ./montador -p <name>.asm


## Compile

Generate binary code (`.OBJ` file).
Perform lexical, syntactical and semantic checks following the defined assembly grammar. Also checks the types of parameters and quantitites according to the respective command.
    
    ./montador -o <name>.PRE



![Tabela de comandos][1]


[1]: images/OPCODE-Table.PNG