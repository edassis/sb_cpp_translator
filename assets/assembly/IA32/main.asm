%include "inout.asm"

section .text
global _start

_start:
    call LerChar
    mov ecx, eax
    mov edx, ebx

    mov eax, SYS_WRITE       ; syscall
    mov ebx, STDOUT          ; file descriptor
    int KERNEL_CALL         ; interruption

    call PrintNwln
    exit