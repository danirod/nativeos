/*
 * This file is part of NativeOS
 * Copyright (C) 2015 Dani Rodríguez <danirod@outlook.com>
 * 
 * File: driver/timer.h
 * Purpose: Timer driver
 */

#ifndef DRIVER_TIMER_H_
#define DRIVER_TIMER_H_

/* Initialize (or reinitialize the timer). */
void timer_init(void);

/* How many ticks have happened since the timer was initialized. */
long timer_get_ticks();

#endif // DRIVER_TIMER_H_