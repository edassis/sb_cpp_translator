sys_exit        equ     1
sys_read        equ     3
sys_write       equ     4
sys_brk         equ     45
stdin           equ     0
stdout          equ     1

section .data
szGreet         db  'Hello!', 0Ah, 'What is your name? :',0
Greet_len       equ $-szGreet     
szColor         db  'What is your favorite color? :',0 
Color_len       equ $ - szColor
szSpace         db ",", 32, 0
suprise1:       db 'No way ',0                             
suprise1L       equ $-suprise1
suprise3:       db ' is my favorite color, too!', 0Ah,0
surprise3L      equ $-suprise3

section .bss
lpName:        resb 20                                      ;user's name
name_len       equ    $ - lpName    
lpColor:       resb 15             
color_len      equ $-lpColor                         ;user's color
TempBuf        resd 1

section .text
    global _start

_start:
    ; mov     edx, name_len 
    ; mov     ecx, lpName
    ; call    ReadText                        ; Read name into buffer
    
    ; mov     edx, name_len 
    ; mov     ecx, lpName
    ; call    ReadText                        ; Read name into buffer
.greeting:
    mov     edx, Greet_len  
    mov     ecx, szGreet 
    call    WriteText                       ; Say hello, ask for name

.getname:    
    mov     edx, name_len 
    mov     ecx, lpName
    call    ReadText                        ; Read name into buffer

.getcolor: 
    mov     edx, Color_len              
    mov     ecx, szColor
    call    WriteText                       ; Ask for fav color

    mov     edx, color_len
    mov     ecx, lpColor
    call    ReadText                        ; Read color into buffer

.GetLen:
    mov     edi, lpName
    call    GetStrlen                       ; length of inputed name
    mov     esi, edx

    mov     edi, lpColor                    ; length of inputed color
    call    GetStrlen

    add     esi, edx                        ; Name len + Color len
    add     esi, suprise1L                  ; surprise1 text len
    add     esi, surprise3L                 ; surprise3 text len
    add     esi, 2                          ; lenght of comma and space
    call    Alloc                           ;
    mov     [TempBuf], eax                  ; address of the end of bss section

.thesuprise:
    push    suprise3
    push    lpColor
    push    szSpace
    push    lpName
    push    suprise1
    push    dword [TempBuf]
    push    5
    call    szMultiCat                      ; concat all strings together
    add     esp, 4 * 7 

    mov     ecx, dword [TempBuf]
    mov     edx, esi
    call    WriteText                       ; print to terminal

    ;~ "free" memory
    mov     ebx, [TempBuf]
    mov     eax, sys_brk                    ; restore system break
    int     80H

    mov     eax, sys_exit
    mov     ebx, 0
    int     80H

;~ #########################################
WriteText:    
    mov     ebx, stdout
    mov     eax, sys_write
    int     80H 
    ret

;~ #########################################    
ReadText:
    mov     ebx, stdin 
    mov     eax, sys_read       
    int     80H    
    ret  

;~ #########################################     
GetStrlen:
    xor     ecx, ecx
    not     ecx
    xor     eax, eax
    cld
    repne   scasb
    mov     byte [edi - 2], 0               ; null terminate string
    not     ecx
    lea     edx, [ecx - 1]
    ret

;~ ######################################### 
Alloc:
    ; esi == size to allcocate

    ;~ Get end of bss section
    xor     ebx, ebx
    mov     eax, sys_brk
    int     80H
    push    eax

    ; extend it by size
    mov     ebx, eax
    add     ebx, esi
    mov     eax, sys_brk
    int     80H

    pop     eax                             ; return address of break
    ret

;  #####################################################################
;  Name       : szMultiCat
;  Arguments  : esp + 4 = 
;               esp + 8 = 
;               esp + 12 = 
;               esp + 16 =
;  Description: Concats multiple NULL terminated strings together
;  Returns    : Nothing
;
szMultiCat: 
    push    ebx
    push    esi
    push    edi
    push    ebp

    mov     ebx, 1

    mov     ebp, [esp + 20]
    mov     edi, [esp + 24]           ; lpBuffer
    xor     ecx, ecx
    lea     edx, [esp + 28]           ; args
    sub     edi, ebx

.B1:
    add     edi, ebx
    movzx   eax, byte [edi]   ; unroll by 2
    test    eax, eax
    je      .nxtstr

    add     edi, ebx
    movzx   eax, byte [edi]
    test    eax, eax
    jne     .B1

.nxtstr:
    sub     edi, ebx
    mov     esi, [edx + ecx * 4]
.B2:
    add     edi, ebx
    movzx   eax, byte [esi]   ; unroll by 2
    mov     [edi], al
    add     esi, ebx
    test    eax, eax
    jz      .F1

    add     edi, ebx
    movzx   eax, byte [esi]
    mov     [edi], al
    add     esi, ebx
    test    eax, eax
    jne     .B2

.F1:
    add     ecx, ebx
    cmp     ecx, ebp                ; pcount
    jne     .nxtstr

    mov     eax, [esp + 24]           ; lpBuffer

    pop     ebp
    pop     edi
    pop     esi
    pop     ebx
    ret