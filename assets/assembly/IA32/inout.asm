; global LerInteiro, EscreverInteiro
; global LerChar, EscreverChar
; global LerString, EscreverString
; global Overflow

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

section .data
    nwln        db      0dh, 0ah
    NWLN_SIZE    equ     $-nwln

    msg1 db "Foram lidos ",
    MSG1_SIZE equ $-msg1
    msg_chars db " caracteres.",0
    MSG_CHARS_SIZE equ $-msg_chars
    msg_nums db " algarismos.",0
    MSG_NUMS_SIZE equ $-msg_nums

    msg_overflow db "Overflow! Finalizando o programa...",0
    MSG_OVERFLOW_SIZE equ $-msg_overflow
    
    BUFFER_MAX_SIZE equ 100
    buffer_len dw 0

section .bss
    buffer  resb 100
    r_char  resb 1
    tmp    resd 10

section .text
    ; global _start

; finishes execution
Overflow:
    mov eax, msg_overflow
    mov ebx, MSG_OVERFLOW_SIZE
    call _Print
    call _PrintNwln
    exit

; not safe
; eax - end. mem
; ebx - len
_Print:
    mov ecx, eax
    mov edx, ebx
    mov eax, SYS_WRITE      ; syscall
    mov ebx, STDOUT          ; file descriptor
    int KERNEL_CALL         ; interruption
    ret

; not safe
; eax - end. mem
; ebx - len
_Scan:
    mov ecx, eax    ; len
    mov edx, ebx
    mov eax, SYS_READ      ; syscall
    mov ebx, STDIN          ; file descriptor
    int KERNEL_CALL         ; interruption
    ret

; not safe
_PrintNwln:
    mov eax, SYS_WRITE       ; syscall
    mov ebx, STDOUT          ; file descriptor
    mov ecx, nwln            ; end.
    mov edx, NWLN_SIZE       ; len
    int KERNEL_CALL         ; interruption
    ret

; not safe
; stack param1 = qtd, param2 = 0/1
_PrintLidos:
    push ebp
    mov ebp, esp
    
    mov eax, msg1
    mov ebx, MSG1_SIZE
    call _Print

    mov eax, [ebp+12]           ; ebp | eip | param2 | param1
    sub esp, BUFFER_MAX_SIZE*2  ; words
    push eax
    call _IntToString
    mov esp, eax
    shl esp, 1      ; words
    neg esp
    add esp, ebp    ; new TOS

    mov ecx, eax    ; len
    mov eax, esp    ; TOS
.l1:
    push eax
    push ecx    ; len

    mov ecx, eax
    mov edx, 1
    mov eax, SYS_WRITE       ; syscall
    mov ebx, STDOUT          ; file descriptor
    int KERNEL_CALL          ; interruption
    
    pop ecx
    pop eax
    add eax, 2  ; pop one word
    loop .l1

    mov eax, [ebp+8]    ; param2
    cmp eax, 1
    je .nums
.chars: ; eax - 0
    mov eax, msg_chars
    mov ebx, MSG_CHARS_SIZE
    call _Print
    jmp .end
.nums: ; eax - 1
    mov eax, msg_nums
    mov ebx, MSG_NUMS_SIZE
    call _Print
.end:
    call _PrintNwln

    mov esp, ebp
    pop ebp
    ret 8

; stack: param1 - value
; return: values in stack, eax - len
_IntToString:    ; stack = x return; 200 bytes
    push ebx
    push ecx
    push edx
    push ebp
    mov ebp, esp  

    xor ecx, ecx    ; counter

    mov eax, dword [ebp+20]             ; original value
    push eax             
    mov ebx, ebp                        ; base address
    add ebx, BUFFER_MAX_SIZE*2+24       ; +16 - eip | param1 | ret
    push ebx

    cmp eax, 0      ; if value < 0
    jl .negative
.l1:
    xor edx, edx
    mov ebx, 10     ; divisor
    div ebx         ; eax = quotient, edx = remainder
.digit:  
    add edx, '0'    ; convert remainder to char
    
    inc ecx         ; counter++

    mov edi, ecx
    neg edi
    shl edi, 1
    mov ebx, [ebp-8]
    mov word [ebx+edi], dx   ; ret base address *(ebp-8) + offset
    
    cmp ecx, BUFFER_MAX_SIZE
    jg .negative2             ; obs: overflow in the future
.quo:
    cmp eax, 0    
    jne .l1             ; has quotient?
    jmp .negative2      ; end
.negative:  ; if number is negative
    neg eax
    jmp .l1
.negative2: ; adds leading '-' on TOS
    cmp dword [ebp-4], 0
    jge .end        ; if original value >= 0

    inc ecx

    mov edi, ecx
    neg edi
    shl edi, 1
    mov ebx, [ebp-8]
    mov word [ebx+edi], '-'  ; ret base address *(ebp-8) + offset
.end:
    mov eax, ecx        ; len

    mov esp, ebp        ; recover stack
    pop ebp
    pop edx
    pop ecx
    pop ebx
    ret 4

; stack: param1 - address, param2 - len
; return value on stack
_StringToInt: 
    pusha       ; +32
    mov ebp, esp

    xor ecx, ecx
    xor ebx, ebx
.l1:
    cmp ecx, dword [ebp+36]    ; param2
    je .end

    mov eax, dword [ebp+40]    ; param1 = address
    movzx eax, byte [eax+ecx]   ; char
    sub eax, '0'

    inc ecx
    
    cmp eax, 0      ; if value == 0
    jle .l1
    cmp eax, 9
    jg .l1
    
    push ecx
.l2:
    cmp ecx, dword [ebp+36]   ; param2
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
    mov eax, dword [ebp+40]
    cmp byte [eax], '-'     ; first char is '-' ?
    jne .end2
    neg ebx
.end2:
    mov [ebp+44], ebx    ; +32 - eip | param1 | param2 | ret 

    mov esp, ebp
    popa
    ret 8

; -------------------------------------------------------
; Subroutines
; -------------------------------------------------------
; param1 - address, param2 - max. len
; return eax - len
LerString:
    push ebx
    push ecx
    push edx
    push ebp    ; +16 - eip | param2 | param1
    mov ebp, esp
    
    mov eax, dword [ebp+24]     ; address
    xor ebx, ebx                ; offset
    mov ecx, dword [ebp+20]     ; qtd to read, max = 100
    cmp ecx, 0
    jle .setmax
    cmp ecx, BUFFER_MAX_SIZE
    jle .l1
.setmax:
    mov ecx, BUFFER_MAX_SIZE
.l1:
    push eax
    push ebx
    push ecx
    
    mov eax, r_char
    mov ebx, 1
    call _Scan
    
    pop ecx                 ; unstack before jump
    pop ebx
    pop eax

    cmp byte [r_char], 0ah
    je .cond
    cmp byte [r_char], ' '  ; space 32
    jb .l1
    cmp byte [r_char], '~'  ; 126
    ja .l1

    mov dl, [r_char]
    mov [eax+ebx], dl   ; buffer[i] = char
    inc ebx
    loop .l1
.cond:
    cmp ebx, 0
    je .l1
.end:
    push ebx
    
    push ebx
    push dword 0
    call _PrintLidos
    
    pop ebx
    mov eax, ebx    ; len

    mov esp, ebp
    pop ebp
    pop edx
    pop ecx
    pop ebx
    ret 8

; stack: param1 - end.mem., param2 - len
EscreverString:
    pusha
    mov eax, [esp+40]
    mov ebx, [esp+36]
    call _Print
    call _PrintNwln
    popa
    ret 8

; stack: param1 - address
; return eax - len
LerChar:
    pusha
    mov ebp, esp

    mov eax, dword [ebp+36] ; address
    xor ecx, ecx            ; counter
.l1:
    push eax
    push ecx

    mov eax, r_char
    mov ebx, 1
    call _Scan

    pop ecx
    pop eax

    cmp byte [r_char], 0ah
    je .cond
    cmp byte [r_char], ' '  ; 32
    jb .l1
    cmp byte [r_char], '}'  ; 126
    ja .l1
    
    mov bl, [r_char]
    mov [eax], bl
    inc ecx
.cond:
    cmp ecx, 0
    je .l1
.end:
    push dword 1
    push dword 0
    call _PrintLidos

    mov esp, ebp
    popa
    mov eax, 1
    ret 4

; stack: param1 - address
EscreverChar:
    pusha   ; +32
    mov ebp, esp
    
    mov eax, [ebp+36]
    mov ebx, 1
    call _Print
    call _PrintNwln

    mov esp, ebp
    popa
    ret 4

; stack: param1 - address
; return  eax - len
LerInteiro:
    push ebx
    push ecx
    push edx
    push ebp             
    mov ebp, esp
    xor ecx, ecx
    mov ebx, [ebp+20]   ; +16 - eip | param1
.l1:
    push ebx
    push ecx

    mov eax, r_char
    mov ebx, 1
    call _Scan

    pop ecx
    pop ebx

    cmp byte [r_char], 0ah ; \n
    je .cond
    cmp byte [r_char], '-'
    je .write
    cmp byte [r_char], '0'
    jb .l1
    cmp byte [r_char], '9'
    ja .l1
.write:
    mov al, [r_char]
    mov byte [ebx+ecx], al
    inc ecx
    cmp ecx, BUFFER_MAX_SIZE
    jl .l1
.cond:
    cmp ecx, 0
    je .l1
.end:
    push ebx
    push ecx      ; store len, PrintLidos not safe

    push ecx            ; params
    push dword 0
    call _PrintLidos        

    pop ecx
    pop ebx

    sub esp, 4          ; return space
    push ebx            ; address
    push ecx
    call _StringToInt       
    pop eax             ; int value
    mov [ebx], eax      ; putting value in param1 address

    mov eax, ecx        ; ret len

    mov esp, ebp
    pop ebp
    pop edx
    pop ecx
    pop ebx
    ret 4

; stack: param1 - address
EscreverInteiro:
    pusha
    mov ebp, esp
    mov eax, dword [ebp+36]   ; param1
    mov eax, dword [eax]
    sub esp, BUFFER_MAX_SIZE*2
    push eax
    call _IntToString    ; return values on stack, eax - len
    mov esp, eax
    shl esp, 1      ; words
    neg esp
    add esp, ebp    ; new TOS

    mov ecx, eax    ; copy len
    mov eax, esp    ; copy stack with string
.l1:
    push eax
    push ecx

    mov ebx, 1      ; print len 1
    call _Print
    
    pop ecx
    pop eax

    add eax, 2      ; next element
    loop .l1

    call _PrintNwln

    mov esp, ebp
    popa
    ret 4

; ------------------------------------
; Tests for the procedures
; _start:       
    mov eax, 3
    mov ebx, 6
    mov ecx, 9
    mov edx, 12
    mov ebp, 15

    ; CHAR ------------------------
    push buffer
    call LerChar
    mov dword [tmp], eax      ; len
    push tmp
    call EscreverInteiro

    push buffer
    call EscreverChar

    ; INT ------------------------
    push buffer
    call LerInteiro
    mov [tmp], eax
    push tmp
    call EscreverInteiro

    push buffer
    call EscreverInteiro

    ; STRING ------------------------
    push buffer
    push 20
    call LerString
    mov dword [tmp], eax
    push tmp
    call EscreverInteiro

    push buffer
    push eax
    call EscreverString
    
    xor edi, edi
    exit

; -------------------------------------------------