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

#ifndef ARCH_X86_IDT_H_
#define ARCH_X86_IDT_H_

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

/* This is the prototype function for local IDT handlers. */
typedef void (*local_idt_handler_t)(struct idt_data*);

/* This function is used to modify the handler associated to a interrupt. */
void idt_set_handler(unsigned int interrupt_code, local_idt_handler_t handler);

#endif // ARCH_X86_IDT_H_
