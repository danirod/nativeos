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

#include <kernel/cpu/gdt.h>

/* GDT structures. */
struct gdt gdt_toc;

struct gdt_entry gdt_table[3];

/* Load the GDT table. This is defined in lgdt.s */
extern void gdt_load();

static void gdt_set_entry(int sid, unsigned int base, unsigned int limit,
				unsigned char access, unsigned char granularity)
{
	/* Fill the base address (32 bits) */
	gdt_table[sid].base_low = base & 0xFFFF;
	gdt_table[sid].base_middle = (base >> 16) & 0xFF;
	gdt_table[sid].base_high = (base >> 24) & 0xFF;

	/* Fill the segment limit address (20 bits) */
	gdt_table[sid].limit_low = limit & 0xFFFF;
	gdt_table[sid].granularity = (limit >> 16) & 0x0F;

	/* Fill the access and granularity. */
	gdt_table[sid].access = access;
	gdt_table[sid].granularity |= (granularity & 0xF0);
}

void gdt_init()
{
	/* Setup the segments: NULL, code (0x9A) and data (0x92). */
	gdt_set_entry(0, 0, 0, 0, 0);
	gdt_set_entry(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); /* 0x9A = CODEDATA_BIT ON. */
	gdt_set_entry(2, 0, 0xFFFFFFFF, 0x92, 0xCF); /* 0x92 = CODEDATA_BIT OFF.*/

	/* Set up the GDT table. */
	gdt_toc.size = sizeof (struct gdt_entry) * 3 - 1;
	gdt_toc.base = (unsigned int) &gdt_table;

	/* Relocate the GDT table. */
	gdt_load();
}
