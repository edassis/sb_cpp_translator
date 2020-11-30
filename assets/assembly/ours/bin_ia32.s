%include "inout.asm"

section .data
	DOIS dd 2

section .bss
	OLD_DATA resd 1
	NEW_DATA resd 1
	TMP_DATA resd 1

section .text
	global _start

_start:
	push OLD_DATA
	call LerInteiro
	mov eax, dword [OLD_DATA]
L1:
	cdq
	mov ebx, dword [DOIS]
	idiv ebx
	mov dword [NEW_DATA], eax
	mov ebx, dword [DOIS]
	imul ebx
	jo Overflow
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
