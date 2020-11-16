section .data
msg         db      'Hello World',0dh,0ah
SIZE_MSG    EQU     $-msg

section .text
global _start
_start:
    mov eax,4
    mov ebx,1
    mov ecx,msg
    mov edx,SIZE_MSG
    int 80h
    mov eax,1
    mov ebx,0
    int 80h
