        ; 8080 assembler code
        .hexfile test.hex
        .binfile test.com
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

; This program tests all System Calls
; Mehmed Mustafa 131044084 13/03/2017

StringInput: DS 100		; Reserve memory for the an input string

begin:
	LXI SP, stack 		; Always initialize the stack pointer
	LXI B, StringInput	; Initialize reserved memory to BC
	
	; Read a string and store it into Address starting from BC
	; After that print that string on the screen
	MVI A, READ_STR		; Store the OS call code to A
	call GTU_OS			; Call the OS
	MVI A, PRINT_STR	; Store the OS call code to A
	call GTU_OS			; Call the OS

	; Read a number and store it into Register B
	; After that print that number on the screen
	MVI A, READ_B		; Store the OS call code to A
	call GTU_OS			; Call the OS
	MVI A, PRINT_B		; Store the OS call code to A
	call GTU_OS			; Call the OS

	LXI B, 1000H		; Initialize B with 1000H/4096D
						; Read into 1000H/4096D address
	MVI A, READ_MEM 	; Store the OS call code to A
	call GTU_OS			; Call the OS

	LXI B, 11H			; Initialize B with 11H/17D
						; Print the content of 11H/17D address
	MVI A, PRINT_MEM 	; Store the OS call code to A
	call GTU_OS			; Call the OS
	hlt					; end program
