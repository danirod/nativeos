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
 * File: driver/vga.h
 * Description: bogus VGA driver for debug printing
 */

#ifndef DRIVER_VGA_H_
#define DRIVER_VGA_H_

/* VGA color constants. */
enum VGA_COLORS {
	COLOR_BLACK = 0,
	COLOR_BLUE = 1,
	COLOR_GREEN = 2,
	COLOR_CYAN = 3,
	COLOR_RED = 4,
	COLOR_MAGENTA = 5,
	COLOR_BROWN = 6,
	COLOR_LIGHT_GRAY = 7,
	COLOR_DARK_GRAY = 8,
	COLOR_LIGHT_BLUE = 9,
	COLOR_LIGHT_GREEN = 10,
	COLOR_LIGHT_CYAN = 11,
	COLOR_LIGHT_RED = 12,
	COLOR_LIGHT_MAGENTA = 13,
	COLOR_LIGHT_BROWN = 14,
	COLOR_WHITE = 15
};

/* Initialize or reset the VGA console to the initial status. */
void VGACon_Init();

/* Print a character to the console. */
void VGACon_PutChar(char ch);

/* Print a null-terminated string to the console. */
void VGACon_PutString(char *chArr);

/* Move the cursor to an arbitrary position on the screen. */
void VGACon_Gotoxy(int x, int y);

/* Set the color used in future PutChar or PutString calls. */
void VGACon_SetColor(unsigned char fg, unsigned char bg);

/* Clear the console using the current color attributes. */
void VGACon_Clrscr();

/* Delete last char inserted */
void VGACon_Retn();

/* Lock retn at current char */
void VGACon_LockRetn();


#endif // KERNEL_CONSOLE_VGACON_H_
