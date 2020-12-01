# SB-Translator

A C++ translator for the assembly invented in class.


## Translator modes

### Pre-Process

Remove unnecessary spaces, tabs, comments, and do basic code formatting (`.PRE` file).
Checks whether a given command is valid.
    
    ./montador -p <name>.asm


### Assemble

Generate binary code (`.OBJ` file).
Perform lexical, syntactical and semantic checks following the defined assembly grammar. Also checks the types of parameters and quantitites according to the respective command.
    
    ./montador -o <name>.PRE


### Translate

Generate an assembly file in x86 (`.s` file).
As in Assemble option, makes several check on original file before translation. The output file is capable of be compiled
with NASM and executed natively.
    
    ./montador -x86 <name>.asm

![Tabela de comandos][1]


[1]: assets/images/OPCODE-Table.png