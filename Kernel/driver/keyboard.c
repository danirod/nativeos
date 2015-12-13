/*
 * This file is part of NativeOS
 * Copyright (C) 2015 Dani Rodríguez <danirod@outlook.com>
 * 
 * File: driver/keyboard.c
 * Purpose: Keyboard driver
 */

#include <driver/keyboard.h>
#include <kernel/idt.h>
#include <kernel/io.h>

/* This is the handler for the keyboard driver. */
static void keyboard_int_handler(struct idt_data* data)
{
	int scancode = IO_InP(0x60);
	
	/* TODO: Do something with the scancode? */
}

/* Initialize the keyboard system. */
void keyboard_init()
{
	// Set the handler.
	idt_set_handler(0x21, &keyboard_int_handler);
}