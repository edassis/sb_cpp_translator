%include "inout.asm"
section .data
	OITO dd 8

section .bss
	N resd 2

section .text
	global _start

_start:
	push N+4
	call LerInteiro
	mov eax, dword [N+4]
	cdq
	mov ebx, dword [OITO]
	imul ebx
	jo Overflow
	mov dword [N], eax
	push N+4
	call EscreverInteiro
	push N
	call EscreverInteiro
	mov eax, 1
	mov ebx, 0
	int 80h
