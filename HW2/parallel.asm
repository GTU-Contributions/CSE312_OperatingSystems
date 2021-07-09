        ; 8080 assembler code
        .hexfile parallel.hex
        .binfile parallel.com
        ; try "hex" for downloading in hex format
        .download bin  
        .objcopy gobjcopy
        .postbuild echo "OK!"
        ;.nodump

		; OS call list
PRINT_B		equ 1
PRINT_MEM	equ 2
READ_B		equ 3
READ_MEM	equ 4
PRINT_STR	equ 5
READ_STR	equ 6
FORK		equ 7
EXEC		equ 8
WAITPID		equ 9

	; Position for stack pointer
stack   equ 04000h

	org 000H
	jmp begin

	; Start of our Operating System
GTU_OS:	PUSH D
	push D
	push H
	push psw
	nop	; This is where we run our OS in C++, see the CPU8080::isSystemCall()
		; function for the detail.
	pop psw
	pop h
	pop d
	pop D
	ret
	; ---------------------------------------------------------------
	; YOU SHOULD NOT CHANGE ANYTHING ABOVE THIS LINE        

; This program runs 3 processes at the same
; time. One process prints numbers from 0 to 50, the other process 
; add numbers from 1 to 10 and prints the result and final process 
; prints the numbers from 100 to 50.
; Mehmed Mustafa 131044084 27/04/2017

program1: dw 'PrintNumbers.com', 00H		; First program name
program2: dw 'sum.com', 00H					; Second program name
program3: dw 'PrintNumbersRev.com', 00H		; Third program name

begin:
	LXI SP, stack 		; Always initialize the stack pointer
	MVI A, FORK			; Store the OS call code to A
	call GTU_OS			; Call the OS
	MOV D, B			; Save the pid of the child in Reg D
	LXI	B, program1		; Load First program in BC address
	MVI A, EXEC			; Store the OS call code to A
	call GTU_OS			; Call the OS

	MVI A, FORK			; Store the OS call code to A
	call GTU_OS			; Call the OS
	MOV D, B			; Save the pid of the child in Reg D
	LXI	B, program2		; Load Second program in BC address
	MVI A, EXEC			; Store the OS call code to A
	call GTU_OS			; Call the OS

	MVI A, FORK			; Store the OS call code to A
	call GTU_OS			; Call the OS
	MOV D, B			; Save the pid of the child in Reg D
	LXI	B, program3		; Load Third program in BC address
	MVI A, EXEC			; Store the OS call code to A
	call GTU_OS			; Call the OS

	hlt					; end program
	
	
	
	
	
	