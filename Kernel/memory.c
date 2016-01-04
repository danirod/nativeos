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
 * File: kernel/memory.c
 * Description: memory primitives and structures
 */
 
#include <kernel/memory.h>

/**
 * Defined in linker.ld: will point to the end memory address
 * for the kernel. This is, after this address, there is no
 * more kernel.
 */
extern char kernel_after;

/**
 * This is the memory address that will be returned the next
 * time kmalloc is executed. It has to be incremented every
 * time kmalloc is called.
 */
static unsigned char* next_address = &kernel_after;

void *kmalloc(unsigned int size) {
	unsigned char* address_to_return = next_address;
	next_address += size;
	return address_to_return;
}