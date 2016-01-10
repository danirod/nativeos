/*
 * This file is part of NativeOS: next-gen x86 operating system
 * Copyright (C) 2015-2016 Dani Rodríguez
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
 * File: panic.c
 * Description: kill the kernel in case of critical error.
 */

#include <kernel/kernel.h>
#include <driver/vga.h>

/* Defined in bootstrap.s */
extern void kdie();

/*
	Prints a kernel panic message into the framebuffer. The message will
	display the provided error code as an argument. If an extra message
	is given, it will be printed to the screen as well.

	After printing the message, the kdie() function will be invoked.
	Therefore, this function will NOT return because of an infinite loop.
*/

void kpanic(int errcode, char* extra)
{
	VGACon_SetColor(COLOR_LIGHT_GRAY, COLOR_RED);
	VGACon_Clrscr();
	VGACon_Gotoxy(15, 10);
	printk("An error has ocurred and unfortunately NativeOS is");

	VGACon_Gotoxy(15, 11);
	printk("unable to continue normally. Some information might");

	VGACon_Gotoxy(15, 12);
	printk("have been lost.");

	VGACon_Gotoxy(15, 15);
	printk("Kernel Panic:");

	VGACon_Gotoxy(15, 16);
	printk("0x%x ", errcode);
	if (extra) {
		printk("%s", extra);
	}

	VGACon_SetColor(COLOR_RED, COLOR_LIGHT_GRAY);
	VGACon_Gotoxy(35, 7);
	printk(" NativeOS ");

	/* Halt the system. */
	kdie();
}

static char* interrupt_names[] = {
	"Division by zero",
	"Debugger",
	"Non Maskeable Interrupt",
	"Breakpoint",
	"Overflow",
	"Bounds",
	"Invalid Opcode",
	"Coprocessor not available",
	"Double fault",
	"Coprocessor Segment Overrun",
	"Invalid Task State Segment",
	"Segment not Present",
	"Stack Fault",
	"General Protection Fault",
	"Page Fault",
	"Unknown Interruption"
};

void bsod(struct idt_data* data)
{
	VGACon_SetColor(COLOR_LIGHT_GRAY, COLOR_RED);
	VGACon_Clrscr();
	VGACon_Gotoxy(15, 10);
	printk("An error has ocurred and unfortunately NativeOS is");

	VGACon_Gotoxy(15, 11);
	printk("unable to continue normally. Some information might");

	VGACon_Gotoxy(15, 12);
	printk("have been lost.");

	VGACon_Gotoxy(15, 15);
	printk("Stack Trace:");

	VGACon_Gotoxy(15, 16);
	printk("Interrupt Code: 0x%x %s",
			data->int_no, interrupt_names[data->int_no]);

	VGACon_Gotoxy(15, 17);
	printk("EAX = %x  EBX = %x  ECX = %x  EDX = %x",
			data->eax, data->ebx, data->ecx, data->edx);

	VGACon_Gotoxy(15, 18);
	printk("ESI = %x  EDI = %x  EBP = %x  ESP = %x",
			data->esi, data->edi, data->ebp, data->ebp);

	VGACon_SetColor(COLOR_RED, COLOR_LIGHT_GRAY);
	VGACon_Gotoxy(35, 7);
	printk(" NativeOS ");
}
