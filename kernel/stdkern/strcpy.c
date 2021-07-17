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
	while (*src) {
		*ptr++ = *src++;
	}
	return dst;
}

char *
strncpy(char *dst, const char *src, size_t len)
{
	char *ptr = dst;
	while (len--) {
		/* Assigns (and copies) the \0 before failing the test. */
		if (!(*ptr++ = *src++)) {
			break;
		}
	}
	while (len--) {
		/* Fill with zeros until the length is reached. */
		*ptr++ = 0;
	}
	return dst;
}
