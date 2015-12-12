/*
	This file is part of NativeOS
	Copyright (C) 2015 Dani Rodríguez

	idt.h - header file related to IDT set up.
*/

#ifndef KERNEL_IDT_H_
#define KERNEL_IDT_H_

struct idt_table {
	unsigned short limit; /* Size of the IDT table. */
	unsigned int base; /* Base address of the IDT table. */
} __attribute__((packed));

struct idt_entry {
	unsigned short offset1; /* 15..0 bytes for the offset address. */
	unsigned short segment; /* GDT segment for this interrupt. */
	unsigned char zeros; /* Must be zero. */
	unsigned char attributes; /* IDT entry attributes. */
	unsigned short offset2; /* 31..16 bytes for the offset address. */
} __attribute__((packed));

struct idt_data {
	unsigned int edi, esi, ebp, esp;
	unsigned int eax, ebx, ecx, edx;
	unsigned int int_no, err_code;
} __attribute__((packed));

/* This function will set up the IDT table. */
void idt_init(void);

/* This function will handle an interrupt. */
void idt_handler(struct idt_data* data);

#endif // KERNEL_IDT_H_