/*
	This file is part of NativeOS
	Copyright (C) 2015 Dani Rodríguez

	idt.c - sets up the IDT table
*/

#include <kernel/idt.h>
#include <driver/vga.h>
#include <kernel/kernel.h>

#define INTERRUPTS 32

/* Table of contents for the IDT structure. */
struct idt_table idt_toc;

/* Actual IDT data. */
struct idt_entry idt_entries[INTERRUPTS];

/* This is the function that actually loads the IDT table. */
extern void idt_load(void);

/* These are the interrupt definitions. */
extern void isr_0(void);
extern void isr_1(void);
extern void isr_2(void);
extern void isr_3(void);
extern void isr_4(void);
extern void isr_5(void);
extern void isr_6(void);
extern void isr_7(void);
extern void isr_8(void);
extern void isr_9(void);
extern void isr_10(void);
extern void isr_11(void);
extern void isr_12(void);
extern void isr_13(void);
extern void isr_14(void);
extern void isr_15(void);
extern void isr_16(void);
extern void isr_17(void);
extern void isr_18(void);
extern void isr_19(void);
extern void isr_20(void);
extern void isr_21(void);
extern void isr_22(void);
extern void isr_23(void);
extern void isr_24(void);
extern void isr_25(void);
extern void isr_26(void);
extern void isr_27(void);
extern void isr_28(void);
extern void isr_29(void);
extern void isr_30(void);
extern void isr_31(void);

static unsigned int isr_vector[] = {
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
	(unsigned int) &isr_31
};

static void idt_set_entry(int pos, unsigned int offset,
		unsigned short segment, unsigned char attributes)
{
	idt_entries[pos].offset1 = offset & 0xFFFF;
	idt_entries[pos].offset2 = (offset >> 16) & 0xFFFF;
	idt_entries[pos].zeros = 0;
	idt_entries[pos].segment = segment;
	idt_entries[pos].attributes = attributes;
}

void idt_init()
{
	/* Create the IDT table. */
	idt_toc.base = (unsigned int) &idt_entries;
	idt_toc.limit = (sizeof (struct idt_entry) * 256) - 1;
	/* TODO: Fill the IDT table with zeros to avoid unexpected things. */
	
	/* Fill the IDT table. */
	int i;
	for (i = 0; i < INTERRUPTS; i++) {
		idt_set_entry(i, isr_vector[i], 0x08, 0x8E);
	}
	
	/* Actually load the IDT table. */
	idt_load();
}

void idt_handler(struct idt_data* data)
{
	VGACon_SetColor(COLOR_LIGHT_CYAN, COLOR_BLACK);
	printk("==== INTERRUPT ====\n");
	printk("Interrupt Number: 0x%x\n", data->int_no);
	
	printk("EAX = %x  EBX = %x  ECX = %x  EDX = %x\n",
			data->eax, data->ebx, data->ecx, data->edx);
	printk("ESI = %x  EDI = %x  EBP = %x  ESP = %x\n",
			data->esi, data->edi, data->esp, data->ebp);
	printk("==== END OF INTERRUPT ====\n");
	VGACon_SetColor(COLOR_WHITE, COLOR_BLACK);
	
	if (data->int_no < 16) {
		kdie();
	}
}