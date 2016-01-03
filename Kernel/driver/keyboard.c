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
 * File: driver/keyboard.c
 * Description: keyboard driver.
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
