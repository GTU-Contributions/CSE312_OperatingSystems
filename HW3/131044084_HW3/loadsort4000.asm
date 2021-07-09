        ; 8080 assembler code
        .hexfile loadsort4000.hex
        .binfile loadsort4000.com
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
; time. Each process
; Mehmed Mustafa 131044084 28/05/2017

program: dw 'sortv4000.com', 00H		; The program to be forked

begin:
	LXI SP, stack 		; Always initialize the stack pointer

	MVI A, FORK		; Store the OS call code to A
	call GTU_OS		; Call the OS
	MVI A, 0		; Clear A
	CMP B			; Check if B is 0 or contains PID
	JNZ nextFork	; if this is the parent jump to the nextFork
	LXI B, program	; Load the program in BC address
	MVI A, EXEC		; Store the OS call code to A
	call GTU_OS		; Call the OS
	JMP HALT		; end child

nextFork:
	MVI B, 0		; Clear B
	MVI A, FORK		; Store the OS call code to A
	call GTU_OS		; Call the OS
	MVI A, 0		; Clear A
	CMP B			; Check if B is 0 or contains PID
	JNZ  nextFork2	; if this is the parent jump to the nextFork
	LXI B, program	; Load the program in BC address
	MVI A, EXEC		; Store the OS call code to A
	call GTU_OS		; Call the OS
	JMP HALT		; end child

nextFork2:
	MVI B, 0		; Clear B
	MVI A, FORK		; Store the OS call code to A
	call GTU_OS		; Call the OS
	MVI A, 0		; Clear A
	CMP B			; Check if B is 0 or contains PID
	JNZ  HALT		; if this is the parent jump to the nextFork
	LXI B, program	; Load the program in BC address
	MVI A, EXEC		; Store the OS call code to A
	call GTU_OS		; Call the OS
	JMP HALT		; end child

HALT:
	hlt			; end program
	
	
	
	