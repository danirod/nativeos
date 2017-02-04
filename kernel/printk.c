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
 * File: printk.c
 * Description: kernel printf, always prints to VGA screen.
 */

#include <stdarg.h>
#include <driver/vga.h>

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

/* This function will print an unsigned number to the screen. */
static void printk_write_uint(const unsigned int number)
{
	/* Prepare the buffer. */
	char buf[20];
	buf[0] = 0;
	int len = 0;

	/* Reverse write the number to the char array. */
	unsigned int value = number;
	while (value >= 10) {
		int last_digit = value % 10;
		value /= 10;
		buf[++len] = '0' + last_digit;
	}
	buf[++len] = '0' + value;

	/* Reverse print the char array. */
	while (len) {
		VGACon_PutChar(buf[len--]);
	}
}

static char hex_letters[] = {
	'0', '1', '2', '3', '4', '5', '6', '7',
	'8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
};

static void printk_write_hex(const unsigned int number)
{
	char buf[20];
	buf[0] = 0;
	int len = 0;

	/* Write the number to the buffer. */
	unsigned int value = number;
	while (value >= 16) {
		buf[++len] = hex_letters[value & 0xF];
		value >>= 4;
	}
	buf[++len] = hex_letters[value & 0xF];

	/* Reverse print the buffer. */
	while (len) {
		VGACon_PutChar(buf[len--]);
	}
}

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
				unsigned int u_num;
				char* d_str;
				ch++;
				switch (*ch) {
					case 'c': // print a character.
						d_num = va_arg(list, int);
						VGACon_PutChar(d_num);
						break;
					case 'd': // print a number
						d_num = va_arg(list, int);
						printk_write_int(d_num);
						break;
					case 's': // print a string.
						d_str = va_arg(list, char*);
						VGACon_PutString(d_str);
						break;
					case 'u':
						u_num = va_arg(list, unsigned int);
						printk_write_uint(u_num);
						break;
					case 'x':
						u_num = va_arg(list, unsigned int);
						printk_write_hex(u_num);
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