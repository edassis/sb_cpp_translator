%include "inout.asm"

section .data
	ONE dd 1

section .bss
	AUX resd 1
	N resd 1

section .text
	global _start

_start:
	push N
	call LerInteiro
	mov eax, dword [N]
FAT:
	sub eax, dword [ONE]
	cmp eax, 0
	je FIM
	mov dword [AUX], eax
	cdq
	mov ebx, dword [N]
	imul ebx
	cmp edx, 0
	jne _Overflow
	mov dword [N], eax
	mov eax, dword [AUX]
	jmp FAT
FIM:
	push N
	call EscreverInteiro
	mov eax, 1
	mov ebx, 0
	int 80h
