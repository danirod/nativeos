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
 * File: driver/vga.c
 * Description: bogus VGA driver for debug printing
 */

#include <arch/x86/io.h>
#include <driver/vga.h>

/* If it's set to 1, all the VGA output will get redirected to the serial port */
#define DEBUG 1

#if DEBUG
#include <driver/com.h>
#endif

/* How many characters fit in a row of text. */
#define CONSOLE_COLS 80

/* How many character lines fit in the screen. */
#define CONSOLE_ROWS 25

/* How many characters in total fit in the screen. */
#define CONSOLE_SIZE (CONSOLE_COLS * CONSOLE_ROWS)

/* Macro for creating VGA entries with character, foreground and background */
#define VGA_ENTRY(char, fg, bg) (char | fg << 8 | bg << 12)

/* Address register for the VGA controller. */
#define VGACON_ADDR_REG 0x3D4

/* Data register for the VGA controller. */
#define VGACON_DATA_REG 0x3D5

/* Cursor Location (high and low register) */
#define VGACON_COMMAND_CURSOR_HI 0xE
#define VGACON_COMMAND_CURSOR_LO 0xF

/* Base address for the framebuffer. 16 bit pointer to include color attrs */
static unsigned short *baseAddr;

/* Current character position. */
static int cursorX, cursorY;

/* Last locked pos in console (don't delete characters after \n) */
int last_locked_pos = 0;

/* Current foreground and background color. */
static unsigned char fgColor, bgColor;

/*
	This function will scroll the screen either up or down a number of times
	provided in the amount argument. The direction of the scroll will depend
	on the sign of the argument. If the sign is positive, the scroll is done
	upwards (new lines enter on the bottom, top lines are lost). If the
	sign is negative, the scroll is done downwards (new lines enter on the
	top and bottom lines are lost).
*/
void Scroll(int amount)
{
	/* Start and end pointers for the region that will be overwritten. */
	unsigned short *startAddr, *endAddr;

	/* Offset for the source copying. */
	short offset;

	/** Start and end pointers for the region that will be cleant. */
	unsigned short *baseClean, *endClean;

	if (amount > 0) {
		/* TODO: Should this variable mess be cleaned? */
		startAddr = baseAddr;
		endAddr = startAddr + CONSOLE_COLS * (CONSOLE_ROWS - amount);
		offset = CONSOLE_COLS * amount;
		baseClean = endAddr;
		endClean = baseAddr + CONSOLE_COLS * CONSOLE_ROWS;

		/* Overwrite one region of memory with other. TODO: memcpy? */
		unsigned short* copyAddr;
		for (copyAddr = startAddr; copyAddr < endAddr; copyAddr++) {
			*(copyAddr) = *(copyAddr + offset);
		}

		/* Now clean the memory region that has scrolled in. TODO: memset? */
		unsigned short* cleanAddr;
		for (cleanAddr = baseClean; cleanAddr < endClean; cleanAddr++) {
			*(cleanAddr) = VGA_ENTRY(0, fgColor, bgColor);
		}
	} else if (amount < 0) {
		startAddr = baseAddr + (-amount * CONSOLE_COLS);
		endAddr = startAddr + CONSOLE_COLS * (CONSOLE_ROWS + amount);
		offset = amount * CONSOLE_COLS;
		baseClean = startAddr + offset;
		endClean = startAddr - 1;

		/* Overwrite one region of memory with other. */
		unsigned short *copyAddr;
		for (copyAddr = endAddr; copyAddr >= startAddr; copyAddr--) {
			*(copyAddr) = *(copyAddr + offset);
		}

		/* Clean the memory region. */
		unsigned short *cleanAddr;
		for (cleanAddr = baseClean; cleanAddr < endClean; cleanAddr++) {
			*(cleanAddr) = VGA_ENTRY(0, fgColor, bgColor);
		}
	}
}

/*
	This function will relocate the framebuffer blinking cursor via IO
	operations to place it in the position described by the cursor
	variables. It should be called whenever the cursor has changed so
	that the display renders the blinking line where it should.
*/
static void UpdateFramebufferCursor()
{
	int pos = cursorY * CONSOLE_COLS + cursorX;
	IO_OutP(VGACON_ADDR_REG, VGACON_COMMAND_CURSOR_HI);
	IO_OutP(VGACON_DATA_REG, ((pos >> 8) & 0xFF));
	IO_OutP(VGACON_ADDR_REG, VGACON_COMMAND_CURSOR_LO);
	IO_OutP(VGACON_DATA_REG, pos & 0xFF);
}

/*
	Increment the cursor indices and possibly update the cursor position
	on the screen. After invoking this method, the cursor static variables
	that dictate where will the next character be printed out will be
	updated.
*/
static void IncrementCursor()
{
	if (++cursorX == CONSOLE_COLS) {
		cursorX = 0;
		if (++cursorY == CONSOLE_ROWS) {
			/* Scroll the cursor, then put it on the just made line. */
			Scroll(1);
			cursorY = CONSOLE_ROWS - 1;
		}
	}

	/* Move the blinking cursor by calling the appropiate VGA routines. */
	UpdateFramebufferCursor();
}

/*
	Initialize or reinitialize the console status. This function should be
	invoked by the kernel or the kernel bootstrapper at least once before
	attempting to use the screen to initialize the state.

	If the kernel ever needs to reinitialize the console it can use this
	method to clear the screen. Note that init will wipe the current
	foreground and background colors and replace them with light gray over
	black background.
*/
void VGACon_Init()
{
	/* This is the base memory address for video RAM. */
	baseAddr = (unsigned short *) 0x000B8000;

	/* Set up the console state. */
	cursorX = 0, cursorY = 0;
	fgColor = COLOR_LIGHT_GRAY;
	bgColor = COLOR_BLACK;

	/* Empty the memory and put the cursor in the top left corner. */
	register int i;
	for (i = 0; i < CONSOLE_SIZE; i++)
		*(baseAddr + i) = VGA_ENTRY(0, fgColor, bgColor);

	/* Move the blinking cursor to the top left as well. */
	UpdateFramebufferCursor();
}

/*
	Print a character to the screen and update the cursor position. Note
	that this function will print the provided character by directly
	copying the character into the framebuffer. This method does not
	support escaped characters such as '\n' or '\t'.
*/
void VGACon_PutChar(char ch)
{
#if DEBUG
	serial_send_byte(COM_PORT_1, ch);
#else
	register int pos;

	switch (ch) {
		case '\n':
			/* Introduce a line break. */
			cursorX = 0;
			if (++cursorY == CONSOLE_ROWS) {
				Scroll(1);
				cursorY = CONSOLE_ROWS - 1;
			}
			UpdateFramebufferCursor();
			VGACon_LockRetn();
			break;
		case '\r':
			/* Line feed */
			cursorX = 0;
			UpdateFramebufferCursor();
			break;
		default:
			/* Place the character in the frame buffer. */
			pos = cursorY * CONSOLE_COLS + cursorX;
			*(baseAddr + pos) = VGA_ENTRY(ch, fgColor, bgColor);
			IncrementCursor();
			break;
	}
#endif
}

/*
	Print a null-terminated string to the screen. The cursor position will
	be updated as characters are printed by. Note that this function will
	print raw the string by copying the data to the framebuffer. This
	method doesn't support escaped characters and it's not an equivalent
	form of printf.
*/
void VGACon_PutString(char* chArray)
{
#if DEBUG
	serial_send_str(COM_PORT_1, chArray);
#else
	register char *ch;
	register int pos;

	/* Iterate until we find a NUL character. */
	for (ch = chArray; *ch; ch++) {
		/* What are the performance implications of such loop? */
		VGACon_PutChar(*ch);
	}
#endif
}

/*
	Move the cursor position to the one given as arguments. Future calls to
	VGACon_PutChar or VGACon_PutString will use the new cursor position as
	the base position for start printing characters.
*/
void VGACon_Gotoxy(int x, int y)
{
	/* Move the cursor coordinates. */
	cursorX = x;
	cursorY = y;

	/* Update the cursor position in the frame buffer. */
	UpdateFramebufferCursor();
}

/*
	This function will modify the current foreground color and background
	color that is in use on VGACon_PutChar and VGACon_PutString methods
	and replace them by the ones given here.
*/
void VGACon_SetColor(unsigned char fg, unsigned char bg)
{
	fgColor = fg;
	bgColor = bg;
}

void VGACon_Clrscr()
{
	/* Reset cursor position. */
	cursorX = 0;
	cursorY = 0;
	UpdateFramebufferCursor();

	/* Clear the console. */
	register int i;
	for (i = 0; i < CONSOLE_SIZE; i++)
		*(baseAddr + i) = VGA_ENTRY(0, fgColor, bgColor);
}

/* Delete last char */
void VGACon_Retn()
{
	register int pos;

	int _cursorX = cursorX, _cursorY = cursorY;

	if (--_cursorX < 0) {
		_cursorY -= 1;
		_cursorX = CONSOLE_COLS - 1;
	}

	pos = _cursorY * CONSOLE_COLS + _cursorX;

	if (pos >= last_locked_pos) {
		cursorX = _cursorX;
		cursorY = _cursorY;
		
		*(baseAddr + pos) = VGA_ENTRY('\0', fgColor, bgColor);
	}
} 

/* Lock retn before next position */
void VGACon_LockRetn()
{
	last_locked_pos = cursorY * CONSOLE_COLS + cursorX;
}