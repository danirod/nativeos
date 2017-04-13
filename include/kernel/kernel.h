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
 * File: kernel/kernel.h
 * Description: main kernel include file
 */

#ifndef KERNEL_KERNEL_H_
#define KERNEL_KERNEL_H_

#include <arch/x86/idt.h>

/* printk: like printf, but for the kernel and with less placeholders. */
void printk(char* fmt, ...);

/* kpanic: print a kernel panic message and halt the system. */
void kpanic(int errcode, char* extra);

void bsod(struct idt_data* data);

#endif // KERNEL_KERNEL_H_
