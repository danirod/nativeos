/*
	This file is part of NativeOS.
	Copyright (C) 2015 Dani Rodríguez

	vgacon.c - Bogus display driver for the VGA framebuffer.
*/

#include <kernel/console/vgacon.h>

/* How many characters fit in a row of text. */
#define CONSOLE_COLS 80

/* How many character lines fit in the screen. */
#define CONSOLE_ROWS 24

/* How many characters in total fit in the screen. */
#define CONSOLE_SIZE (CONSOLE_COLS * CONSOLE_ROWS)

/* Macro for creating VGA entries with character, foreground and background */
#define VGA_ENTRY(char, fg, bg) (char | fg << 8 | bg << 12)

/* Base address for the framebuffer. 16 bit pointer to include color attrs */
static unsigned short *baseAddr;

/* Current character position. */
static int cursorX, cursorY;

/* Current foreground and background color. */
static unsigned char fgColor, bgColor;

/*
	Increment the cursor indices and possibly update the cursor position
	on the screen. After invoking this method, the cursor static variables
	that dictate where will the next character be printed out will be
	updated.

	At the moment the VGA Console driver doesn't support scrolling. If it
	is ever implemented, it will allow to scroll up the console when the
	cursor reaches the bottom of the screen. At the moment the cursor
	simply jumps to the top.

	At the moment the VGA Console driver cannot update the blinking bar
	that is displayed on the screen to indicate where will the next
	character be printed out.
*/
static void IncrementCursor()
{
	if (++cursorX == CONSOLE_COLS) {
		cursorX = 0;
		if (++cursorY == CONSOLE_ROWS) {
			/* TODO: Wouldn't it be better to scroll up the window? */
			cursorY = 0;
		}
	}

	/*
		TODO: Update the blinking caret position on the screen.
		I cannot do this at the moment because I need IO routines
		to talk to the framebuffer using ports.
	*/
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
}

/*
	Print a character to the screen and update the cursor position. Note
	that this function will print the provided character by directly
	copying the character into the framebuffer. This method does not
	support escaped characters such as '\n' or '\t'.
*/
void VGACon_PutChar(char ch)
{
	/* Places a character in the framebuffer. */
	register int pos = cursorY * CONSOLE_COLS + cursorX;
	*(baseAddr + pos) = VGA_ENTRY(ch, fgColor, bgColor);

	// Increment the cursor position.
	IncrementCursor();
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
	register char *ch;
	register int pos;

	// Iterate until we find a NUL-character.
	for (ch = chArray; *ch; ch++) {
		// Put this character to the framebuffer.
		pos = cursorY * CONSOLE_COLS + cursorX;
		*(baseAddr + pos) = VGA_ENTRY(*ch, fgColor, bgColor);

		// Increment the cursor position.
		IncrementCursor();
	}
}

/*
	Move the cursor position to the one given as arguments. Future calls to
	VGACon_PutChar or VGACon_PutString will use the new cursor position as
	the base position for start printing characters.
*/
void VGACon_Gotoxy(int x, int y)
{
	cursorX = x;
	cursorY = y;
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
