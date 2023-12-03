# SB Translator

A C++ translator for the assembly invented in class.


## Translator modes

### Pre-Process

Remove unnecessary spaces, tabs, comments, and do basic code formatting (`.PRE` file).
Validate specified commands.
    
    ./montador -p <name>.asm


### Assemble

Generate binary code (`.OBJ` file).
Perform lexical, syntactical and semantic checks following the defined assembly grammar. Also checks the types of parameters and quantitites according to the respective command.
    
    ./montador -o <name>.PRE


### Translate

Generate an assembly file in x86 (`.s` file).
As in Assemble option, makes several checks on original file before translation. The output file can be compiled
with NASM to produce a native executable.
    
    ./montador -x86 <name>.asm

![Commands table][1]


[1]: assets/images/OPCODE-Table.png
