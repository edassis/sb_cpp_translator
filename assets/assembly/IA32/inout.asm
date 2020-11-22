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

    msg1 db "Foram lidos ",
    MSG1_SIZE equ $-msg1
    msg_chars db " caracteres.",0
    MSG_CHARS_SIZE equ $-msg_chars
    msg_nums db " algarismos.",0
    MSG_NUMS_SIZE equ $-msg_nums

    BUFFER_MAX_SIZE equ 100
    buffer_len dw 0

section .bss
    buffer resb 100
    r_char resb 1

section .text
    global _start

; not safe
; eax - end. mem
; ebx - len
Print:
    mov ecx, eax
    mov edx, ebx
    mov eax, SYS_WRITE      ; syscall
    mov ebx, STDOUT          ; file descriptor
    int KERNEL_CALL         ; interruption
    ret

; not safe
; eax - end. mem
; ebx - len
Scan:
    mov ecx, eax    ; len
    mov edx, ebx
    mov eax, SYS_READ      ; syscall
    mov ebx, STDIN          ; file descriptor
    int KERNEL_CALL         ; interruption
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
; eax - 0 char, 1 num
PrintLidos:
    push eax
    push ebp
    mov ebp, esp
    
    mov eax, msg1
    mov ebx, MSG1_SIZE
    call Print

    movzx eax, word [buffer_len]

    sub esp, BUFFER_MAX_SIZE*2  ; words
    call IntToString
    
    mov ecx, eax    ; len
    mov ebx, 1
.l1:
    push ebx    ; counter
    push ecx    ; len

    neg ebx     ; next position on stack, -counter*2
    shl ebx, 1
    add ebx, ebp

    mov ecx, ebx
    mov edx, 1
    mov eax, SYS_WRITE       ; syscall
    mov ebx, STDOUT          ; file descriptor
    int KERNEL_CALL          ; interruption
    
    pop ecx
    pop ebx
    inc ebx         ; increment array word
    cmp ebx, ecx
    jle .l1

    mov eax, [ebp+4]    ; first push -----
    cmp eax, 1
    je .nums
.chars: ; eax - 0
    mov eax, msg_chars
    mov ebx, MSG_CHARS_SIZE
    call Print
    jmp .end
.nums: ; eax - 1
    mov eax, msg_nums
    mov ebx, MSG_NUMS_SIZE
    call Print
.end:
    call PrintNwln

    mov esp, ebp
    pop ebp
    add esp, 4      ; rm first push
    ret 

; eax - value
; values into stack, eax - len
IntToString:    ; stack = x return; 200 bytes
    push ebx
    push ecx
    push edx
    push ebp
    mov ebp, esp  

    xor ecx, ecx    ; counter
.l1:
    xor edx, edx
    mov ebx, 10     ; divisor
    div ebx         ; al = quotient, ah = remainder
 
    cmp edx, 0    
    jge .digit  ; has remainder?
.quo:
    cmp eax, 0    
    jne .l1         ; has quotient?
    jmp .inverse    ; the number is inverted, we need to uninvert (stack, ecx, ebp)
.digit:  
    add edx, '0'     ; convert remainder to char
    push dx
    inc ecx
    cmp ecx, BUFFER_MAX_SIZE
    jl .quo
.inverse: 
    mov ebx, ebp    ; base address
    add ebx, BUFFER_MAX_SIZE*2+20     ; +16 | eip | ret
    mov edx, ecx    ; copy
.inverse_l1: 
    pop ax          ; value
    sub ebx, 2
    mov word [ebx], ax
    loop .inverse_l1
.end:
    mov eax, edx        ; len

    mov esp, ebp        ; recover stack
    pop ebp
    pop edx
    pop ecx
    pop ebx
    ret

; access buffer
; return value on stack
StringToInt: 
    pusha       ; +32
    mov ebp, esp

    xor ecx, ecx
    xor ebx, ebx
.l1:
    cmp cx, word [buffer_len]
    je .end

    movzx eax, byte [buffer+ecx]
    sub eax, '0'

    inc ecx
    
    cmp eax, 0      ; if value == 0
    jle .l1
    cmp eax, 9
    jg .l1
    
    push ecx
.l2:
    cmp cx, word [buffer_len]  ; j
    je .store
    
    mov edx, 10
    mul edx
    inc ecx
    jmp .l2
.store:
    add ebx, eax
    pop ecx
    jmp .l1
.end:
    mov [ebp+36], ebx    ; +32 | eip | value 

    mov esp, ebp
    popa
    ret

; -------------------------------------------------------
; Subroutines
; -------------------------------------------------------
; eax - qtd max
; return: eax - address, ebx - len !!!!!!
LerString:
    pusha
    mov word [buffer_len], 0
    mov byte [r_char], 0
    mov ecx, eax   ; qtd to read, max = 100
    cmp ecx, 0
    jle .setmax
    cmp ecx, BUFFER_MAX_SIZE
    jle .l1
.setmax:
    mov ecx, BUFFER_MAX_SIZE
.l1: 
    push ecx
    mov eax, r_char
    mov ebx, 1
    call Scan 
    pop ecx                 ; unstack before jump
    
    cmp byte [r_char], 0ah
    je .cond
    cmp byte [r_char], ' '  ; space 32
    jb .l1
    cmp byte [r_char], '~'  ; 126
    ja .l1

    movzx ebx, word [buffer_len]
    mov al, [r_char]
    mov [buffer+ebx], al   ; buffer[i] = char
    inc word [buffer_len]
    loop .l1
.cond:
    cmp word [buffer_len], 0
    je .l1
.end:
    mov eax, 0
    call PrintLidos
    
    popa
    mov eax, buffer
    movzx ebx, word [buffer_len]
    ret

; end.mem./len in stack
EscreverString:
    pusha
    mov eax, [esp+40]
    mov ebx, [esp+36]
    call Print
    call PrintNwln
    popa
    ret 8

; return eax - end. mem., ebx - length !!!!
LerChar:
    pusha
    mov word [buffer_len], 0
    mov byte [r_char], 0
.l1:
    mov eax, r_char
    mov ebx, 1
    call Scan
    
    cmp byte [r_char], 0ah
    je .cond
    cmp byte [r_char], ' '  ; 32
    jb .l1
    cmp byte [r_char], '}'  ; 126
    ja .l1
    
    mov al, [r_char]
    mov [buffer], al
    inc word [buffer_len]
.cond:
    cmp word [buffer_len], 0
    je .l1
.end:
    mov eax, 0
    call PrintLidos
    popa
    mov eax, buffer
    movzx ebx, byte [buffer_len]
    ret

; param in stack
EscreverChar:
    pusha   ; +32
    mov ebp, esp
    
    mov eax, ebp
    add eax, 36
    mov ebx, 1
    call Print
    call PrintNwln

    mov esp, ebp
    popa
    ret 4

; return in eax
LerInteiro:
    pusha             ; sub-routines need to use stack too
    mov ebp, esp     
    mov ecx, BUFFER_MAX_SIZE
    mov word [buffer_len], 0
    mov byte [r_char], 0
.l1:
    mov eax, r_char
    mov ebx, 1
    call Scan

    cmp byte [r_char], 0ah ; \n
    je .cond
    cmp byte [r_char], '0'
    jb .l1
    cmp byte [r_char], '9'
    ja .l1

    movzx ebx, word [buffer_len]     ; writes r_char on buffer
    mov al, [r_char]
    mov byte [buffer+ebx], al
    inc word [buffer_len]
    loop .l1
.cond:
    cmp word [buffer_len], 0
    je .l1
.end:
    mov eax, 1
    call PrintLidos        

    mov esp, ebp
    popa 
   
    sub esp, 4          ; return value
    call StringToInt       
    pop eax
    ret

; param in stack
EscreverInteiro:
    pusha
    mov ebp, esp
    mov eax, [ebp+36]   ; param (value)
    sub esp, BUFFER_MAX_SIZE*2
    call IntToString    ; return values on stack, eax - len
    
    mov ebx, eax    ; copy len
    mov ecx, 1
.l1:
    push ecx    ; counter
    push ebx    ; len

    mov eax, ecx
    neg eax
    shl eax, 1
    add eax, ebp    ; array pos = ebp-counter*2

    mov ebx, 1      ; print len 1
    call Print
    
    pop ebx
    pop ecx
    inc ecx
    cmp ecx, ebx
    jle .l1

    call PrintNwln

    mov esp, ebp
    popa
    ret 4

_start:
    mov eax, 3
    mov ebx, 6
    mov ecx, 9
    mov edx, 12
    mov ebp, 15

    call LerChar ; rev
    call LerChar ; rev
    push word [eax]
    call EscreverChar
    
    call LerInteiro
    push eax
    call EscreverInteiro
    
    call LerString ; rev
    push eax
    push ebx
    call EscreverString

    ; mov eax, 97
    ; push eax
    ; call EscreverChar
    exit

; passagem de parametros deve ser feito utilizando a pilha (obrigatorio)
; eax deve retornar a qtd de caracteres lidos (string)
; Ler/Escrever String (end. mem., len)

; proc devem usar a pilha para passagem de paramentros.