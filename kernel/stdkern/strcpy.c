/*
 * This file is part of NativeOS
 * Copyright (C) 2015-2018 The NativeOS contributors
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
 */

#include <stddef.h>

char *
strcpy(char *dst, const char *src)
{
	char *ptr = dst;
	if (src && *src) {
		while (*src) {
			*ptr++ = *src++;
		}
		*ptr++ = 0;
	}
	return dst;
}

char *
strncpy(char *dst, const char *src, size_t len)
{
	char *ptr = dst;

	if (len) {
		do {
			// When src reaches ends, assigns \0 before fail.
			if (!(*ptr++ = *src++)) {
				// Did you know that the spec says you must
				// fill the remaining of the string with \0?
				// Because you are supposed to copy len
				// bytes.
				while (len--)
					*ptr++ = 0;
				break;
			}
		} while(--len);
	}
	return dst;
}
