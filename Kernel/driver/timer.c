/*
 * This file is part of NativeOS
 * Copyright (C) 2015 Dani Rodríguez <danirod@outlook.com>
 * 
 * File: driver/timer.h
 * Purpose: Timer driver
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