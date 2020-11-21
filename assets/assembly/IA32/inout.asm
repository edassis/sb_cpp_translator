; global LerInteiro, EscreverInteiro
; global LerChar, EscreverChar
; global LerString, EscreverString

SYS_EXIT equ 1
SYS_READ equ 3
SYS_WRITE equ 4
STDIN equ 0
STDOUT equ 1
KERNEL_CALL equ 80h

%macro exit 0
    mov eax,1   ; exit
    mov ebx,0   ; return value
    int 80h
%endmacro

; use only when accessing memory
%macro print 2  
    mov eax, SYS_WRITE      ; syscall
    mov ebx, STDOUT          ; file descriptor
    mov ecx, %1             ; end. msg
    mov edx, %2             ; len
    int KERNEL_CALL         ; interruption
%endmacro

%macro scan 2
    mov eax, SYS_READ      ; syscall
    mov ebx, STDIN          ; file descriptor
    mov ecx, %1             ; dst end.
    mov edx, %2             ; len
    int KERNEL_CALL         ; interruption
%endmacro

section .data
    nwln        db      0dh, 0ah
    NWLN_SIZE    EQU     $-nwln

    msg1 db "Foram lidos ",0h
    MSG1_SIZE equ $-msg1
    msg2 db " caracteres.",0h
    MSG2_SIZE equ $-msg2
    msg3 db "Mensagem de teste.",0h
    MSG3_SIZE equ $-msg3

    BUFFER_MAX_SIZE equ 100
    buffer_len dw 0

section .bss
    buffer resb 100
    r_char resb 1

section .text
    global _start

; eax - end. mem
; ebx - len
Print:
    pusha
    mov ecx, eax
    mov edx, ebx
    mov eax, SYS_WRITE      ; syscall
    mov ebx, STDOUT          ; file descriptor
    int KERNEL_CALL         ; interruption
    popa
    ret

; eax - end. mem
; ebx - len
Scan:
    pusha
    mov ecx, eax    ; len
    mov edx, ebx
    mov eax, SYS_READ      ; syscall
    mov ebx, STDIN          ; file descriptor
    int KERNEL_CALL         ; interruption
    popa
    ret

; not safe
PrintNwln:
    mov eax, SYS_WRITE       ; syscall
    mov ebx, STDOUT          ; file descriptor
    mov ecx, nwln            ; end.
    mov edx, NWLN_SIZE       ; len
    int KERNEL_CALL         ; interruption
    ret

; not safe
PrintLidos:
    mov eax, msg1
    mov ebx, MSG1_SIZE
    call Print

    mov eax, [buffer_len]
    call StringToDecimal
    mov ecx, eax
    mov edx, ebx
    mov eax, SYS_WRITE       ; syscall
    mov ebx, STDOUT          ; file descriptor
    int KERNEL_CALL          ; interruption

    mov eax, msg2
    mov ebx, MSG2_SIZE
    call Print

    call PrintNwln
    ret

; eax - value
; return eax end., ebx length
; not safe
StringToDecimal:
    xor ecx, ecx    ; counter
    push ebp
    mov ebp, esp    
    sub esp, BUFFER_MAX_SIZE*2    ; 200 bytes na pilha
.l1: 
    mov ebx, 10     ; divisor
    div bl         ; al = quotient, ah = remainder
 
    cmp ah, 0    
    jge .digit  ; has remainder?
.quo:
    movzx ax, al    ; clear remainder from ah
    cmp ax, 0    
    jne .l1         ; has quotient?
    jmp .inverse    ; the number is inverted, we need to uninvert (stack, ecx, ebp)
.digit: 
    movzx bx, ah  
    add bx, '0'     ; convert remainder to char
    push bx
    inc ecx
    cmp ecx, BUFFER_MAX_SIZE
    jl .quo
.inverse: 
    xor ebx, ebx
.inverse_l1: 
    pop ax
    mov byte [ebp+ebx], al
    inc ebx
    cmp ebx, ecx
    jl .inverse_l1
.end: 
    mov eax, ebp         ; eax = end.mem, ebx = len
    mov ebx, ecx
    mov esp, ebp         ; recover stack
    pop ebp              ; recover ebp
    ret

; not safe
LerString: 
    mov ecx, eax   ; qtd to read, max = 100
    cmp ecx, 0
    jle .setmax
    cmp ecx, BUFFER_MAX_SIZE
    jle .l1
.setmax:
    mov ecx, BUFFER_MAX_SIZE
.l1: 
    push ecx    ; this isn't needed anymore I think, Scan do pusha
    mov eax, r_char
    mov ebx, 1
    call Scan 
    pop ecx                 ; unstack before jump
    
    cmp byte [r_char], 0ah  ; \n
    je .end
    movzx ebx, word [buffer_len]
    mov al, [r_char]
    mov [buffer+ebx], al   ; buffer[i] = char
    inc word [buffer_len]
    loop .l1
.end: call PrintLidos
    mov eax, buffer
    mov ebx, [buffer_len]
    ret

; eax - end. mem, ebx - length
; not safe
EscreverString:
    call Print
    call PrintNwln
    ret

; return eax - end. mem., ebx - length
; not safe
LerChar:
    mov eax, r_char
    mov ebx, 1
    call Scan
    cmp byte [r_char], 0ah  ; \n
    je LerChar
    
    mov al, [r_char]
    mov [buffer], al
    mov word [buffer_len], 1
    
    call PrintLidos

    mov eax, buffer
    mov ebx, [buffer_len]
    ret

; eax - end. mem
; not safe
EscreverChar: 
    mov ebx, 1
    call Print
    call PrintNwln
    ret

LerInteiro: ; TODO

EscreverInteiro: ; TODO

_start:
    call LerString ; eax contains the address with data
    mov ecx, eax
    mov edx, ebx

    ; call LerChar
    ; mov ecx, eax
    ; mov edx, ebx

    call EscreverString

    mov eax, 'L'
    push eax
    mov eax, esp
    call EscreverChar
    add esp, 4

    mov eax, msg3
    mov ebx, MSG3_SIZE
    call EscreverString

    exit
