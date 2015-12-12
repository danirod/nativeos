; This file is part of NativeOS
; Copyright (C) 2015 Dani Rodríguez
;
; idt.s - actually set up the IDT table.

global idt_load
extern idt_toc
extern idt_handler

; This function will enable the IDT table. It reads the IDT table from the
; global variable idt_toc. This is not wrong, the table has to be in a
; known and fixed memory address. Although I might consider providing the
; pointer as an argument to avoid extern declarations.
idt_load:
	lidt [idt_toc]
	ret

%macro NON_ERROR_INTERRUPT 1
	global isr_%1
	isr_%1:
		push 0 ; error code
		push %1 ; interrupt code0
		jmp idt_common_prehandler
%endmacro

%macro ERROR_INTERRUPT 1
	global isr_%1
	isr_%1:
		push %1 ; interrupt code0
		jmp idt_common_prehandler
%endmacro

NON_ERROR_INTERRUPT 0
NON_ERROR_INTERRUPT 1
NON_ERROR_INTERRUPT 2
NON_ERROR_INTERRUPT 3
NON_ERROR_INTERRUPT 4
NON_ERROR_INTERRUPT 5
NON_ERROR_INTERRUPT 6
NON_ERROR_INTERRUPT 7
ERROR_INTERRUPT 8
NON_ERROR_INTERRUPT 9
ERROR_INTERRUPT 10
ERROR_INTERRUPT 11
ERROR_INTERRUPT 12
ERROR_INTERRUPT 13
ERROR_INTERRUPT 14
NON_ERROR_INTERRUPT 15
NON_ERROR_INTERRUPT 16
NON_ERROR_INTERRUPT 17
NON_ERROR_INTERRUPT 18
NON_ERROR_INTERRUPT 19
NON_ERROR_INTERRUPT 20
NON_ERROR_INTERRUPT 21
NON_ERROR_INTERRUPT 22
NON_ERROR_INTERRUPT 23
NON_ERROR_INTERRUPT 24
NON_ERROR_INTERRUPT 25
NON_ERROR_INTERRUPT 26
NON_ERROR_INTERRUPT 27
NON_ERROR_INTERRUPT 28
NON_ERROR_INTERRUPT 29
NON_ERROR_INTERRUPT 30
NON_ERROR_INTERRUPT 31

idt_common_prehandler:
	; Push data
	pusha
	
	; Create a pointer to ESP so that we can access
	; the data using a struct pointer in C.
	mov eax, esp
	push eax
	
	call idt_handler
	
	pop eax ; Pop the pointer
	popa	; Pop the registers
	add esp, 8 ; Remove the error code and the interrupt
	ret ; TODO: Change with iret once this works.