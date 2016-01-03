/*
 * This file is part of NativeOS: next-gen x86 operating system
 * Copyright (C) 2015-2016 Dani Rodríguez
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
 *
 *
 * File: main.c
 * Description: main kernel routine for the NativeOS Kernel.
 */

#include <kernel/kernel.h>
#include <kernel/gdt.h>
#include <kernel/idt.h>
#include <kernel/multiboot.h>
#include <driver/keyboard.h>
#include <driver/timer.h>
#include <driver/vga.h>

/*
	This is the main routine for the NativeOS Kernel. It will start the
	system and jump to user mode so that the init process can run.
	At the moment no information is gathered from multiboot but I expect
	this to change in the near future.

	Multiboot will provide two arguments here: one is the magic number,
	which must be 0x2BADB002, and the other one is the data structure with
	information that might be required for some things.
*/
void kmain(unsigned int magic_number, multiboot_info_t *multiboot_ptr)
{
	gdt_init();
	idt_init();

	int i;
	for (i = 0; i < 16; i++)
		idt_set_handler(i, &bsod);

	/* Set up the core drivers. */
	VGACon_Init();
	keyboard_init();
	timer_init();

	/* Check that the magic code is valid. */
	if (magic_number != 0x2BADB002) {
		kpanic(0x88, "Wrong magic number");
	}

	printk("Starting NativeOS...\n");

	for(;;);
}
