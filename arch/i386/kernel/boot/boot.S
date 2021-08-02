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

/* Magic number used by the multiboot bootloader. */
#define MULTIBOOT_MAGIC_NUMBER 0x1BADB002

/* Ask for memory alignment and memory information. */
#define MULTIBOOT_FLAGS 0x00000003

/* Provide a valid checksum. */
#define MULTIBOOT_CHECKSUM -(MULTIBOOT_MAGIC_NUMBER + MULTIBOOT_FLAGS)

/* Stack allocation. */
#define STACK_SIZE 0x4000

/*
 * Marking the section as allocatable and executable (AX) will cause
 * both GCC and LLVM to place the multiboot section inside the 4 KB
 * file sector required by the Multiboot spec.
 */
.section .multiboot, "a"
multiboot_header:
	.int MULTIBOOT_MAGIC_NUMBER
	.int MULTIBOOT_FLAGS
	.int MULTIBOOT_CHECKSUM

.text
.align 4
.global Bootstrap
Bootstrap:
/* Test that the magic number is correct, so that we can be free from this.
   GNU GRUB must have placed the value 0x2badb002 in the EAX register.
   If this is not the case, something is very wrong with this bootloader
   and we should not boot. */
        cmpl $0x2badb002, %eax
        je .BootstrapContinueBooting
        jmp kdie

    .BootstrapContinueBooting:
/* Set up the stack. */
        movl $(Stack + STACK_SIZE), %esp

/* Store the multiboot info in a kernel structure. */
    .extern multiboot_info;
    movl %ebx, multiboot_info;

	/* Set up GDT */
	lgdt gdt_toc
	ljmp $0x8, $.after_gdt_setup

.after_gdt_setup:
	/* Execute the kernel. */
	call kmain

/* Enters an infinite loop with interrupts disabled. There is no back from
   this function unless you reboot your system. Should only be used whenever
   the kernel enters a non-recoverable state. */
   .global kdie
    kdie:
        cli
    .dieloop:
        hlt
        jmp .dieloop

.section .rodata
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
.align 4
    .lcomm Stack, STACK_SIZE
