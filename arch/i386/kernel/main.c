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

/**
 * @file
 * @brief NativeOS main initialization code
 *
 * If you are reading this because you want to understand what happens when
 * NativeOS starts -- this is not the actual entrypoint. The main entrypoint
 * is the Bootstrap function located in boot.s. It contains some initialization
 * routines that can only be done in Assembly such as setting up the stack
 * before calling kmain. However, virtually you can consider the kmain function
 * the main entrypoint for NativeOS.
 */

#include <kernel/cpu/gdt.h>
#include <kernel/cpu/idt.h>
#include <kernel/multiboot.h>
#include <kernel/paging.h>
#include <kernel/mem/heap.h>

/**
 * @brief Main routine for the NativeOS Kernel.
 *
 * This function can be considered the main entrypoint after the bootstrap
 * function in boot.s extracts the multiboot data from the processor registers
 * and places them onto the stack in order to provide them as arguments.
 * This kernel will assume that it has been loaded by a multiboot compatible
 * bootloader such as GRUB. There is some checking to assert that but little
 * effort is made.
 *
 * According to section 3.2 of the Multiboot 0.6.96 specification, available
 * at https://www.gnu.org/software/grub/manual/multiboot/multiboot.html,
 * the magic number value must equal 0x2BADB002. The structure will contain
 * values that are set by the bootloader as described on section 3.3 of the
 * specification.
 *
 * @param magic_number the magic number provided by the bootloader.
 * @param multiboot_ptr a pointer to a multiboot structure.
 */
void kmain(multiboot_info_t *multiboot_ptr)
{
	// Init hardware.
	gdt_init();
	idt_init();
	heap_init();
	frames_init(multiboot_ptr);
	for(;;);
}
