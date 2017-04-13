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

#include <arch/x86/gdt.h>
#include <arch/x86/idt.h>
#include <arch/x86/paging.h>
#include <kernel/kernel.h>
#include <kernel/memory.h>
#include <kernel/multiboot.h>
#include <driver/com.h>
#include <driver/keyboard.h>
#include <driver/timer.h>
#include <driver/vga.h>

extern int kernel_start;
extern int kernel_after;

#define LOG(msg) (serial_send_str(COM_PORT_1, msg));

/** 
 * @brief Calculate the amount of memory that this computer has.
 * @param multiboot_info Multiboot structure (it knows everything).
 * @return the amount of KB this PC has or -1 if we cannot know.
 */
static int count_memory(multiboot_info_t *multiboot_info)
{
    if (multiboot_info->flags & 0x01) {
        unsigned int start = multiboot_info->mem_lower;
        /* Multiboot reports the ending area minus 1 MB. Increment it. */
        unsigned int end = multiboot_info->mem_upper + 1024;
        return end - start;
    } else {
        return -1; /* We cannot know. */
    }
}

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
	serial_init(COM_PORT_1, 3);
	LOG("NativeOS x86\n");
	LOG("Serial logging interface is now enabled on port COM1\n");

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
		LOG("PANIC: Wrong multiboot magic number! Check your bootloader.\n");
		for(;;);
	}

    unsigned int memory_amount = count_memory(multiboot_ptr);
    frames_init(memory_amount);

	LOG("NativeOS is starting...\n");

	/* Dumb echo system for bytes read through COM1. */
	for(;;) {
		while (serial_get_recv_status(COM_PORT_1)) {
			unsigned char byte = serial_recv_byte(COM_PORT_1);
			serial_send_byte(COM_PORT_1, byte);
		}
	}
}
