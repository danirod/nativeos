/*
 * This file is part of NativeOS: next-gen x86 operating system
 * Copyright (C) 2015-2016 Dani Rodríguez, 2017-2018 Izan Beltrán <izanbf1803@gmail.com>
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

#include <arch/x86/idt.h>
#include <arch/x86/io.h>
#include <driver/keyboard.h>

char shift_pressed = 0;
char bloq_mayus = 0;
char altgr = 0;

/* This is the handler for the keyboard driver. */
static void keyboard_int_handler(struct idt_data* data)
{
	int scancode = IO_InP(0x60);
	int code = scancode;

	if (code > 128) {
		code -= 128;
		switch (code) {
			case KEY_ALTGR:
				altgr = 0;
				break;
			case KEY_SHIFTL:
				shift_pressed = 0;
				break;
		}
	}
	else {
		//printk("\ncode: %d\n", code);
		switch (code) {
			case KEY_ALTGR:
				altgr = 1;
			case KEY_BLOQ_MAYUS:
				bloq_mayus = !bloq_mayus;
				break;
			case KEY_SHIFTR:
			case KEY_SHIFTL:
				shift_pressed = 1;
				break;
			case KEY_RET:	// RET
				VGACon_Retn();
				break;
			case KEY_TAB: {	// TAB
				int i;
				for (i = 0; i < 4; i++) VGACon_PutChar(' ');
				break;
			}
			case KEY_INSERT:
				VGACon_PutChar('\n');
				// TODO: implement input evaluation
				VGACon_PutChar('>');
				VGACon_LockRetn();
				break;
				
			default:
				if (altgr && keyboard_dict_altgr[code] != '\0')
					VGACon_PutChar(keyboard_dict_altgr[code]);
				else if (shift_pressed && keyboard_dict_shift[code] != '\0')
					VGACon_PutChar(keyboard_dict_shift[code]);
				else if (bloq_mayus && keyboard_dict_mayus[code] != '\0')
					VGACon_PutChar(keyboard_dict_mayus[code]);
				else
					VGACon_PutChar(keyboard_dict[code]);
		}
	}
	//if (code <= 128) printk("scancode: %d\n", scancode);
	/* TODO: Do something with the scancode? */
}

/* Initialize the keyboard system. */
void keyboard_init()
{
	keyboard_map();
	// Set the handler.
	idt_set_handler(0x21, &keyboard_int_handler);
}
