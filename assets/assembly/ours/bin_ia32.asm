; --------------------
; Original - translation to x86 as comments
; --------------------
SECTION TEXT
; section .text
    INPUT OLD_DATA  ; was to be LerChat, but we know that he wants to get integers
    ; push OLD_DATA
    ; call LerInteiro
    LOAD OLD_DATA               ; 6
    ; mov ax, word [OLD_DATA]

L1: DIV DOIS                    ; 3 = OLD_DATA / 2
; L1: cdq
    ; mov ebx, dword [DOIS]
    ; idiv bx
    STORE  NEW_DATA             ; new_data = 3
    ; mov dword [NEW_DATA], eax
    MULT DOIS                   ; 6 = 3 * 2
    ; cdq
    ; mov ebx, dword [DOIS]
    ; imul ebx
    ; cmp edx, 0
    ; jne Overflow
    STORE TMP_DATA              ; TMP_DATA = acc (6)
    ; mov dword [TMP_DATA], eax
    LOAD OLD_DATA               ; acc = OLD_DATA (6)
    ; mov eax, dword [OLD_DATA]
    SUB TMP_DATA                 ; OLD_DATA - TMP_DATA (0)
    ; sub eax, dword [TMP_DATA]
    STORE TMP_DATA              ; TMP_DATA = acc (0)
    ; mov dword [TMP_DATA], eax
    OUTPUT TMP_DATA             ; "0"
    ; push TMP_DATA
    ; call EscreverInteiro
    COPY NEW_DATA, OLD_DATA     ; OLD_DATA = NEW_DATA (3)
    ; mov ebx, dword [NEW_DATA]
    ; mov dword [OLD_DATA], ebx
    LOAD OLD_DATA               ; acc = 3
    ; mov eax, dword [OLD_DATA]
    JMPP L1                     ; if acc > 0
    ; cmp eax, 0
    ; jg L1
    STOP
    ; mov eax, 1
    ; mov ebx, 0
    ; int 80h

SECTION DATA
; section .data
    DOIS: CONST 2       
    ; DOIS dd 2
; section .bss
    OLD_DATA: SPACE  
    ; OLD_DATA resd 1
    NEW_DATA: SPACE
    ; NEW_DATA resd 1
    TMP_DATA: SPACE
    ; TMP_DATA resd 1

; each address is double word length

; mul: eax*<multiplier> = edx:eax
; div: eax/<divisor> = quotient -> eax | remainder -> edx

; --------------------
; IA32 translation
; --------------------
section .data
    DOIS dd 2

section .bss
    OLD_DATA resd 1
    NEW_DATA resd 1
    TMP_DATA resd 1

section .text
_start:
    push OLD_DATA
    call LerInteiro
    mov eax, dword [OLD_DATA]

L1: cdq
    mov ebx, dword [DOIS]
    idiv ebx
    mov dword [NEW_DATA], eax
    cdq
    mov ebx, dword [DOIS]
    imul ebx
    cmp edx, 0
    jne Overflow
    mov dword [TMP_DATA], eax
    mov eax, dword [OLD_DATA] 
    sub eax, dword [TMP_DATA]
    mov dword [TMP_DATA], eax
    push TMP_DATA
    call EscreverInteiro
    mov ebx, dword [NEW_DATA]
    mov dword [OLD_DATA], ebx
    mov eax, dword [OLD_DATA]
    cmp eax, 0
    jg L1
    
    mov eax, 1
    mov ebx, 0
    int 80h