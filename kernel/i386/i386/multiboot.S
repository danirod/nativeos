/*
 * This file is part of NativeOS
 * Copyright (C) 2015-2022 The NativeOS contributors
 * SPDX-License-Identifier:  GPL-3.0-only
 */

/**
 * \file
 * \brief Multiboot entrypoints
 *
 * If the kernel is built with support for multiboot, this file contains
 * the multiboot header as expected to be present by the bootloader in the
 * final executable, and some routines used to test and initialise the
 * multiboot information.
 */

/* Magic number used by the multiboot bootloader. */
#define MULTIBOOT_MAGIC_NUMBER 0x1BADB002

/* Ask for memory alignment and memory information. */
#define MULTIBOOT_FLAGS 0x00000003

	/* Mark the section as allocatable and executable for clang. */
	.section .multiboot, "a"
multiboot_header:
	.int MULTIBOOT_MAGIC_NUMBER
	.int MULTIBOOT_FLAGS
	.int -(MULTIBOOT_MAGIC_NUMBER + MULTIBOOT_FLAGS)

	.text

	.global multiboot_init
	.type multiboot_init, @function

	.extern kernel_die

/**
 * This subroutine should be called as soon as possible because it depends
 * on the proper parameters placed in the CPU registers to succeed. This
 * subroutine will store the multiboot data structure in multiboot_info if
 * the magic number is valid. If the magic number is not valid, the system
 * will halt for your safety.
 *
 * Registers:
 * - EAX: the multiboot magic number (should be set by the bootloader)
 * - EBX: a pointer to the multiboot info (should be set by the bootloader)
 *
 * Returns: nothing, but it will not return if the magic number is not valid.
 */
multiboot_init:
	push %ebp
	mov %esp, %ebp

	/* Test that the magic number is correct as soon as possible. */
	cmpl $0x2badb002, %eax
	je .valid_magic_number
	jmp kernel_die
.valid_magic_number:
	/* The number is valid, the multiboot_info can be trusted. */
	movl %ebx, multiboot_info

	mov %ebp, %esp
	pop %ebp
	ret

.data
	.global	multiboot_info
multiboot_info:
	.int	0
