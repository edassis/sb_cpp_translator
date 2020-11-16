;PURPOSE: Simple program that exits and returns a status code back to the
;Linux kernel.
;INPUT: none.
;OUTPUT: returns a status code. This can be viewed by typing echo $? after
;running the program.
;VARIABLES:
; eax holds the system call number
; ebx holds the return status
section .data
section .text
global _start
_start:
mov eax,1 ;this is the linux system call for exiting a program.
mov ebx,0 ;this is the status number we will return to the operating system.
int 0x80 ;this wakes up the kernel to run the exit command.