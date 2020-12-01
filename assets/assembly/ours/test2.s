%include "inout.asm"

section .data
	ZERO dd 0
	UM dd 1

section .bss
	BUFFER resd 20
	N1 resd 1
	N2 resd 1

section .text
	global _start

_start:
	push BUFFER
	push 100
	call LerString
	push BUFFER
	push 100
	call EscreverString
	push N1
	call LerInteiro
	push N2
	call LerInteiro
	mov eax, dword [N1]
	sub eax, dword [N2]
	mov dword [N1], eax
	push N1
	call EscreverInteiro
	push N1
	call LerInteiro
	mov eax, dword [N1]
L1:
	sub eax, dword [UM]
	mov dword [N1], eax
	push N1
	call EscreverInteiro
	cmp eax, 0
	jg L1
	cmp eax, 0
	je CASE
FIM:
	mov eax, 1
	mov ebx, 0
	int 80h
CASE:
	add eax, dword [UM]
	mov dword [N1], eax
	push N1
	call EscreverInteiro
	sub eax, dword [UM]
	cmp eax, 0
	je FIM
