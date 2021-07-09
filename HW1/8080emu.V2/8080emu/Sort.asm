        ; 8080 assembler code
        .hexfile Sort.hex
        .binfile Sort.com
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

; This program will sort the Numbers: using bubble sort
; And print the result on the screen
; Each number will be printed on a newline.

Numbers: DW 12H,34H,53H,2AH,5BH,6FH,33H,21H,7CH,0FFH,0BAH,0CBH,0A1H,1AH,3BH,0C3H,4AH,5DH,62H,0A3H,0B1H,5CH,7FH,0CCH,0AAH,34H

Begin:
	LXI SP, stack	; Always initialize the SP
	LXI H, Numbers	; Load Numbers in Registers H and L
	MVI C, 26-1		; Initialize C with the number
					; of elements - 1

; Outer Loop
Loop_Out:
	MVI E, 01H		; E holds how many SWAPs occured
					; currently initialized with 1
	MOV B, C		; B holds how many comparisons
					; still to be made in outloop

; Inner Loop
Loop_In:
	MOV A, M		; Load A from memory -> by HL
	INX H 			; +2
	INX H			; Point to the next element
	CMP M			; Compare A and next element
	JC Next			; Jump to NEXT if no swap needed
	
; Swaps two numbers
Swap:
	MOV D, M	
	MOV M, A	 
	DCX H		 
	DCX H		 
	MOV M, D	 
	INX H		 
	INX H		 
	INR E			; ++E, increase swap counter

Next:
	DCR B			; --B
	JNZ Loop_In		; If non-zero, jump to Loop_In to 
					; do next comparisons
	DCR E			; --E, if no swaps occured 
	JZ Exit			; If zero, jump to Exit since 
					; sorting is done

	LXI H, Numbers	; Load Numbers in Registers H and L
	DCR C			; --C
	JNZ Loop_Out	; If non-zero, jump to Loop_Out
					; to do the next number

Exit:
	LXI H, Numbers	; Load Numbers in Registers H and L
	MVI C, 26		; Initialize C with the size 
					; of Numbers

Print:
	MOV B, M		; Move element in HL to B
	MVI A, PRINT_B	; Store the OS call code to A
	call GTU_OS		; Call the OS
	INX H			; +2
	INX H			; Point to the next element
	DCR C			; --C
	JNZ Print		; If non-zero, jump to Print
	hlt				; End the program








