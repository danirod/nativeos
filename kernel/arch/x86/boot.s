; This file is part of NativeOS: next-gen x86 operating system
; Copyright (C) 2015-2016 Dani Rodríguez
;
; This program is free software: you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation, either version 3 of the License, or
; (at your option) any later version.
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with this program.  If not, see <http://www.gnu.org/licenses/>.
;
;
; File: arch/x86/boot.s
; Description: bootstrap code

; Magic number used by the multiboot bootloader.
MULTIBOOT_MAGIC_NUMBER	equ 0x1BADB002

; Ask for memory alignment and memory information.
MULTIBOOT_FLAGS			equ 0x00000003

; Provide a valid checksum.
MULTIBOOT_CHECKSUM		equ -(MULTIBOOT_MAGIC_NUMBER + MULTIBOOT_FLAGS)

; Stack allocation
STACK_SIZE				equ 0x4000

global Bootstrap
global kdie
extern kmain

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

	; Execute the kernel
	push ebx		; Multiboot structure
	push eax		; Magic number
	call kmain		; Call kernel

	; In case the kernel ever returns, kill the system
	jmp kdie

; Enter an infinite loop with interrupts disabled. There is no back from
; this unless you reboot the computer. This should be used in a few
; situations: when the kernel enters a non-recoverable state or when the
; computer should go idle.
kdie:
	cli
.dieloop:
	hlt
	jmp .dieloop

section .bss
align 4
Stack:
	resb STACK_SIZE
