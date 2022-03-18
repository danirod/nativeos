/*
 * This file is part of NativeOS
 * Copyright (C) 2015-2018 The NativeOS contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <config.h>

	.text

	.global kernel_bootstrap
	.type kernel_bootstrap, @function
	.global kernel_die
	.type kernel_die, @function

	.extern idt_init
	.extern heap_init
	.extern pmm_init
	.extern multiboot_init
	.extern kernel_main
	.extern virtual_memory_init

/**
 * This procedure is the actual kernel entrypoint as executed by the bootloader
 * when the system starts. Assembly code is umcomfortable to read and write,
 * so this function should do its stuff as soon as possible and pass control
 * to the portable C code to perform more advanced things.
 */
kernel_bootstrap:
	/* Set up the stack. */
	movl $(kernel_stack + KERNEL_STACK_SIZE), %esp

#ifdef MULTIBOOT
	/* Initialise the multiboot parameters given by the bootloader. */
	call multiboot_init
#endif

	/* Set up GDT */
	lgdt gdt_toc
	ljmp $0x8, $.after_gdt_setup
.after_gdt_setup:
	/* Platform specific initialisation. */
	call idt_init
	call heap_init
	call pmm_init
	call virtual_memory_init

	/* Execute the kernel. */
	call kernel_main

/**
 * This procedure enters an infinite loop of fully halting the system. There
 * is no way this function will return unless the computer is physically
 * switched off or rebooted first.
 */
kernel_die:
	cli
	hlt
	jmp kernel_die

	.rodata
gdt_table:
	/* Entry 0: NULL */
	.long 0 /* (sid = 0, access = 0x00, granularity = 0x00) */
	.long 0 /* (base = 0x00000000, limit = 0x00000000) */

	/* Entry 1: Code Segment */
	.long 0x0000ffff /* (sid = 1, access = 0x9A, granularity = 0xCF) */
	.long 0x00cf9a00 /* (base = 0x00000000, limit = 0xFFFFFFFF) */

	/* Entry 2: Data Segment */
	.long 0x0000ffff /* (sid = 2, access = 0x92, granularity = 0xCF) */
	.long 0x00cf9300 /* (base = 0x00000000, limit = 0xFFFFFFFF) */
gdt_toc:
	.word 0x17
	.long gdt_table

	.bss
	.lcomm kernel_stack, KERNEL_STACK_SIZE
