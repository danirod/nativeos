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

/** 
 * @brief Allocate a memory region in kernel mode.
 *
 * This is the real implementation of the kmalloc function. You'll usually
 * want to use the wrapper functions that provide the valid arguments
 * to the function.
 * 
 * @param size how many bytes do we want to reserve.
 * @param align should the buffer be aligned in a page?
 * @param phys if this is not NULL, return the address here too.
 * 
 * @return pointer to a valid buffer that has been allocated.
 */
static void *kmalloc_real(unsigned int size, int align, unsigned int *phys) {
    /* If the user wants to align this page, do it. */
    if (align && ((unsigned int) next_address & 0xFFF)) {
        next_address = ((unsigned int) next_address & 0xFFFFF000) + 0x1000;
    }

    /* If the user requests the physical memory address, do it. */
    if (phys) {
        *phys = next_address;
    }

    /* Allocate the buffer using a linear system. */
    unsigned char* address_to_return = next_address;
    next_address += size;
    return address_to_return;
}

/** 
 * @brief Allocate a memory buffer (the traditional way).
 * @param size how many bytes to allocate.
 * @return the pointer to the memory buffer.
 */
void *kmalloc(unsigned int size) {
    return kmalloc_real(size, 0, 0);
}

/** 
 * @brief Allocate a memory buffer aligned to the bounds of a new page.
 * @param size how many bytes to allocate.
 * @return the pointer to the memory buffer.
 */
void *kmalloc_al(unsigned int size) {
    return kmalloc_real(size, 1, 0);
}

/** 
 * @brief Allocate a memory buffer.
 * @param size how many bytes to allocate.
 * @param phys the memory address the buffer starts in.
 * @return a pointer to the memory buffer.
 */
void *kmalloc_py(unsigned int size, unsigned int *phys) {
    return kmalloc_real(size, 0, phys);
}

/** 
 * @brief Allocate a memory buffer aligned to the bounds of a new page.
 * @param size how many bytes to allocate.
 * @param phys the memory address the buffer starts in.
 * @return a pointer to the memory buffer.
 */
void *kmalloc_ap(unsigned int size, unsigned int *phys) {
    return kmalloc_real(size, 1, phys);
}
