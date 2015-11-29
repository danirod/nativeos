/*
	This file is part of NativeOS
	Copyright (C) 2015 Dani Rodríguez

	gdt.h - header file related to GDT set up.
*/

#ifndef KERNEL_GDT_H_
#define KERNEL_GDT_H_

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

#endif // KERNEL_GDT_H_
