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
 * File: driver/timer.c
 * Description: PIT controller
 */

#include <driver/timer.h>
#include <kernel/idt.h>

/* How many ticks has the system received. */
static long timer_ticks;

/* Handler for the timer interrupt (0x20). */
static void timer_int_handler(struct idt_data* data)
{
	timer_ticks++;
}

/* Initialize the timer. */
void timer_init()
{
	timer_ticks = 0;
	idt_set_handler(0x20, &timer_int_handler);
}

/* Count how many ticks has the OS received since last init. */
long timer_get_ticks()
{
	return timer_ticks;
}
