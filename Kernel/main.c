/*
	This file is part of NativeOS.
	Copyright (C) 2015 Dani Rodríguez

	main.c - This unit contains the main routine for the NativeOS Kernel.
*/

#include <kernel/kernel.h>
#include <kernel/gdt.h>
#include <kernel/idt.h>
#include <kernel/multiboot.h>
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
	VGACon_Init();
	
	/* Check that the magic code is valid. */
	if (magic_number != 0x2BADB002) {
		kpanic(0x88, "Wrong magic number");
	}
	
	printk("Starting NativeOS...\n");
}