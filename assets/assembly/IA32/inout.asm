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
    msg_chars db " caracteres.",0h
    MSG_CHARS_SIZE equ $-msg_chars
    msg_nums db " algarismos.",0h
    MSG_NUMS_SIZE equ $-msg_nums

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
    ; pusha
    mov ecx, eax
    mov edx, ebx
    mov eax, SYS_WRITE      ; syscall
    mov ebx, STDOUT          ; file descriptor
    int KERNEL_CALL         ; interruption
    ; popa
    ret

; eax - end. mem
; ebx - len
Scan:
    ; pusha
    mov ecx, eax    ; len
    mov edx, ebx
    mov eax, SYS_READ      ; syscall
    mov ebx, STDIN          ; file descriptor
    int KERNEL_CALL         ; interruption
    ; popa
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
    
    mov ecx, ebx    ; len
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

; not safe
; eax - value
; return into stack
IntToString:    ; stack = x return; 200 bytes
    xor ecx, ecx    ; counter
    push ebp
    mov ebp, esp  
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
    add ebx, BUFFER_MAX_SIZE*2+8     ; base position of return array | return | ebp
    mov edx, ecx    ; copy
.inverse_l1: 
    pop ax          ; value
    sub ebx, 2
    mov word [ebx], ax
    loop .inverse_l1
    
    mov ecx, edx
.end:
    mov eax, ebx        ; top of return array
    mov ebx, ecx        ; len

    mov esp, ebp        ; recover stack
    pop ebp             ;
    ret

; not safe
; access buffer
; return value on eax
StringToInt: ; String to int
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
    mov eax, ebx
    ret

; not safe
; eax - qtd max
; return: eax - address, ebx - len
LerString: 
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
    
    cmp byte [r_char], 0ah  ; \n
    je .end

    movzx ebx, word [buffer_len]
    mov al, [r_char]
    mov [buffer+ebx], al   ; buffer[i] = char
    inc word [buffer_len]
    loop .l1
.end:
    mov eax, 0
    call PrintLidos
    mov eax, buffer
    movzx ebx, word [buffer_len]
    ret

; eax - end. mem, ebx - length
EscreverString:
    pusha
    call Print
    call PrintNwln
    popa
    ret

; return eax - end. mem., ebx - length
LerChar:
    pusha
    mov word [buffer_len], 1
.l1:
    mov eax, r_char
    mov ebx, 1
    call Scan
    cmp byte [r_char], 0ah  ; \n
    je .l1
    
    mov al, [r_char]
    mov [buffer], al
    
    mov eax, 0
    call PrintLidos

    popa
    mov eax, buffer
    movzx ebx, byte [buffer_len]
    ret

; eax - end. mem
EscreverChar:
    pusha
    mov ebx, 1
    call Print
    call PrintNwln
    popa
    ret

; not safe
; return value on eax
LerInteiro:
    ; mov byte [r_char], 0
    mov ecx, BUFFER_MAX_SIZE
    ; push ecx
    mov word [buffer_len], 0
.l1:
    mov eax, r_char
    mov ebx, 1
    call Scan

    cmp byte [r_char], 0ah
    je .end
    cmp byte [r_char], '0'
    jb .l1
    cmp byte [r_char], '9'
    ja .l1

    movzx ebx, word [buffer_len]     ; writes r_char on buffer
    mov al, [r_char]
    mov byte [buffer+ebx], al
    inc word [buffer_len]
    loop .l1
.end:
    mov eax, 1
    call PrintLidos        
    call StringToInt
    ret

; not safe
; eax - value
EscreverInteiro:
    push ebp
    mov ebp, esp
    sub esp, BUFFER_MAX_SIZE*2
    call IntToString    ; values on stack, ebx - len
    
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

    add esp, BUFFER_MAX_SIZE*2
    mov esp, ebp
    pop ebp
    ret

_start:
    ; call LerChar
    ; call EscreverChar
    
    ; call LerInteiro
    ; call EscreverInteiro
    
    call LerString
    call EscreverString

    exit

; fazer proc de textos lerem só os ASCIIs validos.
; proc devem usar a pilha para passagem de paramentros e retorno?
; qual padrao seguir para colocar os regs na pilha?
; BUG: calling multiple times scan procs make the last print show
;   the fist scan content.