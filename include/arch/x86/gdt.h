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

#ifndef ARCH_X86_GDT_H_
#define ARCH_X86_GDT_H_

/* Represents the GDT table, this is the pointer used by LGDT */
struct gdt {
	unsigned short size;			/* Number of entries in the GDT table. */
	unsigned int base;				/* Beginning address of the GDT table. */
} __attribute__((packed));

/* Represents a GDT entry in the GDT table. */
struct gdt_entry {
	unsigned short limit_low;		/* GDT 0-15: Segment Limit */
	unsigned short base_low;		/* GDT 16-31: Base Address */
	unsigned char base_middle;		/* GDT 32-39: Base Address */
	unsigned char access;			/* GDT 40-47: Access bits */
	unsigned char granularity;		/* GDT 48-55: Segment Limit */
	unsigned char base_high;		/* GDT 56-63: Base Address */
} __attribute__((packed));

/* Set up the GDT table. */
void gdt_init();

#endif // ARCH_X86_GDT_H_
