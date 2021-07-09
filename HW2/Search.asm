        ; 8080 assembler code
        .hexfile Search.hex
        .binfile Search.com
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

; This program will get an input number
; After that will search that number in the Numbers:
; If found prints the index
; Else error message will be printed
; Mehmed Mustafa 131044084 13/03/2017
	
Numbers: DW 12H,34H,53H,2AH,5BH,6FH,33H,21H,7CH,0FFH,0BAH,0CBH,0A1H,1AH,3BH,0C3H,4AH,5DH,62H,0A3H,0B1H,5CH,7FH,0CCH,0AAH,34H

ErrorStr: DW 'ERROR',00AH,00H ; null terminated error str

Begin:
	LXI SP, stack	; Always initialize the SP
	LXI H, Numbers	; Load Numbers in Registers H and L
	MVI C, 25		; Initialize C with the size 
					; of Numbers - 1
	MVI D, 0		; Index counter

	; Read Integer(Key) into Register B
	MVI A, READ_B	; Store the OS call code to A
	call GTU_OS		; Call the OS
	
Search:
	MOV A, B		; A = B, move the key into Register A
	CMP M			; Compare A with current element
	JZ Found		; If zero, element found, jump to Found
	INR D			; ++Index
	INX H			; +2
	INX H			; Point to the next element
	DCR C			; --C
	JNZ Search		; If non-zero, jump to Search
					; to compare key with the next element
	
Error:
	LXI B, ErrorStr 	; Put the address of error string 
						; in registers B and C
	MVI A, PRINT_STR	; Store the OS call code to A 
	call GTU_OS			; Call the OS 
	JMP Exit			; Jump to Exit
	
Found:
	MOV B, D		; Move the index of the key to Register B
					; into Register B
	MVI A, PRINT_B	; Store the OS call code to A 
	call GTU_OS		; Call the OS 

Exit:
	hlt				; End the program

	








