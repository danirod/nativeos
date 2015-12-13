/*
	This file is part of NativeOS
	Copyright (C) 2015 Dani Rodríguez

	idt.c - sets up the IDT table
*/

#include <kernel/io.h>
#include <kernel/idt.h>
#include <driver/vga.h>
#include <kernel/kernel.h>
#include <kernel/isrdef.h>

/* Table of contents for the IDT structure. */
struct idt_table idt_toc;

/* Actual IDT data. */
struct idt_entry idt_entries[INTERRUPTS];

/* This is the function that actually loads the IDT table. */
extern void idt_load(void);

/* This function will modify one entry in the IDT table. */
static void idt_set_entry(int pos, unsigned int offset,
		unsigned short segment, unsigned char attributes)
{
	idt_entries[pos].offset1 = offset & 0xFFFF;
	idt_entries[pos].offset2 = (offset >> 16) & 0xFFFF;
	idt_entries[pos].zeros = 0;
	idt_entries[pos].segment = segment;
	idt_entries[pos].attributes = attributes;
}

/* Remaps the PIC device to use the given interrupt vector. */
static void remap_pic(unsigned int start_int)
{
	/* Tell the PIC the party is about to begin. */
	IO_OutP(0x20, 0x11);
	IO_OutP(0xA0, 0x11);
	
	/* Tell the PIC where the interrupt vectors start. */
	IO_OutP(0x21, start_int);
	IO_OutP(0xA1, start_int + 8);
	
	/* Tell each PIC which one is master and which one is slave. */
	IO_OutP(0x21, 0x04);
	IO_OutP(0xA1, 0x02);
	
	/* Tell the PICs that this is x86. */
	IO_OutP(0x21, 0x01);
	IO_OutP(0xA1, 0x01);
	
	/* Unmask the PICs. */
	IO_OutP(0x21, 0x00);
	IO_OutP(0xA1, 0x00);
}

/* These are the handlers. */
static local_idt_handler_t idt_handlers[INTERRUPTS];

/*
 * This function can be used to modify which handler is associated with
 * a particular type of interrupt. For instance, the keyboard driver
 * could attach his own handler to the keyboard interrupt in order to
 * receive events when a key is pressed or released.
 */
void idt_set_handler(unsigned int interrupt_code, local_idt_handler_t handler)
{
	if (interrupt_code >= INTERRUPTS) return;
	idt_handlers[interrupt_code] = handler;
}

void idt_init()
{
	/* Create the IDT table. */
	idt_toc.base = (unsigned int) &idt_entries;
	idt_toc.limit = (sizeof (struct idt_entry) * INTERRUPTS) - 1;
	
	/* Remap the PIC. */
	remap_pic(0x20);
	
	/* Fill the IDT table and the handler table. */
	int i;
	for (i = 0; i < INTERRUPTS; i++) {
		idt_set_entry(i, isr_vector[i], 0x08, 0x8E);
		idt_handlers[i] = 0; /* Will use default handler. */
	}
	
	/* Actually load the IDT table. */
	idt_load();
}

/* Fallback handler for interrupts. */
static void fallback_handler(struct idt_data* data)
{
	printk("## Received Interrupt 0x%x ##\n", data->int_no);
}

/* This function is invoked when an interrupt is received. */
void idt_handler(struct idt_data* data)
{
	/* Use the given interrupt handler if exists or use the fallback. */
	if (idt_handlers[data->int_no] != 0) {
		idt_handlers[data->int_no](data);
	} else {
		fallback_handler(data);
	}
	
	/* If this is an exception, I have to halt the system (I think?) */
	if (data->int_no < 16) kdie();
	
	/* Acknowledge the interrupt to the master PIC and possibly slave PIC. */
	if (data->int_no >= 0x28 && data->int_no < 0x30) IO_OutP(0xA0, 0x20);
	if (data->int_no >= 0x20 && data->int_no < 0x30) IO_OutP(0x20, 0x20);
}

/* Vector interrupt table begins here. */
unsigned int isr_vector[] = {
	(unsigned int) &isr_0,
	(unsigned int) &isr_1,
	(unsigned int) &isr_2,
	(unsigned int) &isr_3,
	(unsigned int) &isr_4,
	(unsigned int) &isr_5,
	(unsigned int) &isr_6,
	(unsigned int) &isr_7,
	(unsigned int) &isr_8,
	(unsigned int) &isr_9,
	(unsigned int) &isr_10,
	(unsigned int) &isr_11,
	(unsigned int) &isr_12,
	(unsigned int) &isr_13,
	(unsigned int) &isr_14,
	(unsigned int) &isr_15,
	(unsigned int) &isr_16,
	(unsigned int) &isr_17,
	(unsigned int) &isr_18,
	(unsigned int) &isr_19,
	(unsigned int) &isr_20,
	(unsigned int) &isr_21,
	(unsigned int) &isr_22,
	(unsigned int) &isr_23,
	(unsigned int) &isr_24,
	(unsigned int) &isr_25,
	(unsigned int) &isr_26,
	(unsigned int) &isr_27,
	(unsigned int) &isr_28,
	(unsigned int) &isr_29,
	(unsigned int) &isr_30,
	(unsigned int) &isr_31,
	(unsigned int) &isr_32,
	(unsigned int) &isr_33,
	(unsigned int) &isr_34,
	(unsigned int) &isr_35,
	(unsigned int) &isr_36,
	(unsigned int) &isr_37,
	(unsigned int) &isr_38,
	(unsigned int) &isr_39,
	(unsigned int) &isr_40,
	(unsigned int) &isr_41,
	(unsigned int) &isr_42,
	(unsigned int) &isr_43,
	(unsigned int) &isr_44,
	(unsigned int) &isr_45,
	(unsigned int) &isr_46,
	(unsigned int) &isr_47
};