; This file is part of NativeOS.
; Copyright (C) 2015 Dani Rodríguez
;
; bootstrap.s - program that is loaded by the multiboot bootloader

; Magic number used by the multiboot bootloader.
MULTIBOOT_MAGIC_NUMBER	equ 0x1BADB002

; Ask for memory alignment and memory information.
MULTIBOOT_FLAGS			equ 0x00000003

; Provide a valid checksum.
MULTIBOOT_CHECKSUM		equ -(MULTIBOOT_MAGIC_NUMBER + MULTIBOOT_FLAGS)

; Stack allocation
STACK_SIZE				equ 0x4000

global Bootstrap
extern Kernel_Main

section .text
align 4
MultibootHeader:
	; Put the multiboot data.
	dd MULTIBOOT_MAGIC_NUMBER
	dd MULTIBOOT_FLAGS
	dd MULTIBOOT_CHECKSUM
Bootstrap:
	; Set up the stack.
	mov esp, Stack + STACK_SIZE

	; Prepare to start the kernel.
	push eax
	push ebx
	call Kernel_Main

	cli
.l:	hlt
	jmp .l

section .bss
align 4
Stack:
	resb STACK_SIZE