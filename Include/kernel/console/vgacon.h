/*
	This file is part of NativeOS.
	Copyright (C) 2015 Dani Rodríguez

	vgacon.h - Bogus display driver for the VGA framebuffer.
*/

#ifndef KERNEL_CONSOLE_VGACON_H_
#define KERNEL_CONSOLE_VGACON_H_

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

#endif // KERNEL_CONSOLE_VGACON_H_
