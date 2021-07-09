        ; 8080 assembler code
        .hexfile PrintNumbers.hex
        .binfile PrintNumbers.com
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

	; Position for stack pointer
stack   equ 0F000h

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

; This program prints numbers from 0 to 50 on the screen.
; Each number will be printed on a newline.

currentNumber	ds 2 	; Will keep the current number to be printed

Begin:
	LXI SP, stack		; Always initialize the stack pointer
	MVI A, 0
	STA currentNumber	; Initialize currentNumber to 0
	MVI C, 51			; C = 51, to count how much left	 	
Loop:
	LDA currentNumber	; A = current number to be printed
	MOV B, A			; B = A
	INR A				; ++A, next number to be printed
	STA currentNumber	; Store the next number to be printed
	MVI A, PRINT_B		; Store the OS call code to A
	call GTU_OS			; Call the OS
	DCR C				; --C
	JNZ Loop			; Go to loop if C!=0
	hlt					; End the program