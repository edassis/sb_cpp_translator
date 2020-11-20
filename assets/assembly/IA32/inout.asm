global LerInteiro, EscreverInteiro
global LerChar, EscreverChar
global LerString, EscreverString

SYS_EXIT equ 1
SYS_READ equ 3
SYS_WRITE equ 4
STDIN equ 0
STDOUT equ 1
KERNEL_CALL equ 80h

%macro exit_program 0
    mov eax,1
    mov ebx,0
    int 80h
%endmacro

section .data
    nwln        db      0dh, 0ah
    NWLN_SIZE    EQU     $-nwln

    msg1 db "Foram lidos "
    MSG1_SIZE equ $-msg1

    msg2 db " caracteres."
    MSG2_SIZE equ $-msg2

    BUFFER_SIZE equ 100
    buffer_pos dw 0

section .bss
    buffer resb 100
    char resb 1

section .text
    global _start

LerChar: mov ecx, eax   ; qtd to read, max = 100
    cmp ecx, 100
    jl LerChar_L1
    mov ecx, 100

LerChar_L1: push ecx
    mov eax, SYS_READ       ; syscall
    mov ebx, STDIN          ; file descriptor
    mov ecx, char ; end.
    mov edx, 1    ; len
    int KERNEL_CALL         ; interruption
    
    cmp byte [char], 0ah    ; \n
    je LerChar_END
    mov ebx, buffer_pos
    mov [buffer+ebx*2], char  ; buffer[i] = char
    inc word [buffer_pos]  
    pop ecx
    loop LerChar_L1

LerChar_END: call PrintMsg
    mov eax, [buffer]
    mov ebx, buffer_pos
    add ebx, 1 ; length
    ret


PrintMsg: mov eax, SYS_WRITE       ; syscall
    mov ebx, STDOUT          ; file descriptor
    mov ecx, msg1 ; end.
    mov edx, MSG1_SIZE   ; len
    int KERNEL_CALL         ; interruption

    mov eax, SYS_WRITE       ; syscall
    mov ebx, STDOUT          ; file descriptor
    mov ecx, buffer ; end.
    mov edx, buffer_pos   ; len = pos + 1
    add edx, 1
    int KERNEL_CALL         ; interruption

    mov eax, SYS_WRITE       ; syscall
    mov ebx, STDOUT          ; file descriptor
    mov ecx, msg2 ; end.
    mov edx, MSG2_SIZE   ; len
    int KERNEL_CALL         ; interruption

    call PrintNwln
    ret


PrintNwln: mov eax, SYS_WRITE       ; syscall
    mov ebx, STDOUT          ; file descriptor
    mov ecx, nwln ; end.
    mov edx, NWLN_SIZE   ; len
    int KERNEL_CALL         ; interruption
    
    ret


_start:
    mov eax, 3
    call LerChar ; eax contains the address with data

    mov ecx, eax
    mov edx, ebx

    mov eax, SYS_WRITE       ; syscall
    mov ebx, STDOUT          ; file descriptor
    int KERNEL_CALL         ; interruption

    exit_program
