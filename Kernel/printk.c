/*
	This file is part of NativeOS.
	Copyright (C) 2015 Dani Rodríguez

	printk.c - printf for kernel console, always prints to VGA screen
*/

#include <stdarg.h>
#include <driver/vga.h>

/* Forward declaration: print a number to the console. */
static void printk_write_int(const int number);

/*
 * printk function for NativeOS. Intended to be compatible with Linux's printk.
 * Used by the kernel to print in the console in more creative ways than
 * what VGACon_PutChar or VGACon_PutString offer.
 *
 * First argument is a NUL-terminated string. Succesive arguments should be
 * the contents that the placeholders will be replaced to. At the moment the
 * function knows how to print the following formats:
 *
 * - %d: int numbers
 */
void printk(char* fmt, ...)
{
	if (fmt) { /* Safety nullcheck */
		/* The hardest part here is to extract the variadic argument. */
		va_list list;
		va_start(list, fmt);

		/* Now it's just parsing fmt until we get a NUL character. */
		char* ch;
		for (ch = fmt; *ch; ch++) {
			if (*ch != '%') { /* Not a placeholder, safe to print? */
				VGACon_PutChar(*ch);
			} else { /* Uh, oh, hold on. (Get it? Hold... nevermind) */
				int d_num;
				char* d_str;
				ch++;
				switch (*ch) {
					case 'c': // print a character.
						VGACon_PutChar(*ch);
						break;
					case 'd': // print a number
						d_num = va_arg(list, int);
						printk_write_int(d_num);
						break;
					case 's': // print a string.
						d_str = va_arg(list, char*);
						VGACon_PutString(d_str);
						break;
					default: // unrecognized, what to do?
						VGACon_PutChar('%');
						VGACon_PutChar(*ch);
						break;
				}
			}
		}
	}
}

/*
	Write a numeric value to the console. The algorithm will extract
	numbers from the least significant to the most significant. So,
	the extracted digits will be stored in a string buffer and then
	printed out.
*/
static void printk_write_int(const int number)
{
	/* Initialize the buffer */
	char buf[20];
	buf[0] = 0;
	int len = 0;

	/* Abs conversion. */
	int abs_value = number;
	if (number < 0) {
		abs_value = -number;
	}

	/* Keep putting numbers starting from the right. */
	int last_digit;
	while (abs_value > 9) {
		last_digit = abs_value % 10;
		abs_value /= 10;
		buf[++len] = (char) last_digit + '0';
	}
	buf[++len] = (char) abs_value + '0';

	/* Write the number. */
	if (number < 0) {
		/* Don't forget negative numbers! */
		VGACon_PutChar('-');
	}
	while (len > 0) {
		VGACon_PutChar(buf[len--]);
	}
}
