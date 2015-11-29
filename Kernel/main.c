/*
	This file is part of NativeOS.
	Copyright (C) 2015 Dani Rodríguez

	main.c - This unit contains the main routine for the NativeOS Kernel.
*/

#include <kernel/kernel.h>
#include <kernel/gdt.h>
#include <driver/vga.h>

/*
	This is the main routine for the NativeOS Kernel. It will start the
	system and jump to user mode so that the init process can run.
	At the moment no information is gathered from multiboot but I expect
	this to change in the near future.

	Note for newbies - yes, kmain is a name. There is no OS and therefore
	no rules on how should the main function be named. k* functions are
	related to the kernel.
*/
void kmain()
{
	/* Set up GDT table. */
	gdt_init();

	VGACon_Init();
	printk("Welcome to NativeOS!");

	/* TODO: Replace this with actual kernel code. */
	kpanic(80, "Kernel went idle.");
}
