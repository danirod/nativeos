/*
	This file is part of NativeOS.
	Copyright (C) 2015 Dani Rodríguez

	panic.c - kills the kernel in case of a critical error.
*/

#include <kernel/kernel.h>
#include <driver/vga.h>

/* Defined in bootstrap.s */
extern void kdie();

/*
	Prints a kernel panic message into the framebuffer. The message will
	display the provided error code as an argument. If an extra message
	is given, it will be printed to the screen as well.

	After printing the message, the kdie() function will be invoked.
	Therefore, this function will NOT return because of an infinite loop.
*/

void kpanic(int errcode, char* extra)
{
	/* Set up the VGA framebuffer. */
	VGACon_SetColor(COLOR_WHITE, COLOR_RED);
	VGACon_Gotoxy(0, 0);

	/* Print the message. */
	printk("KERNEL PANIC\n");
	printk("Error code: %d", errcode);
	if (extra) {
		printk("\nMessage: %s", extra);
	}

	/* Halt the system. */
	kdie();
}
