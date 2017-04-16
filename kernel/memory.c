/*
 * This file is part of NativeOS: next-gen x86 operating system
 * Copyright (C) 2015-2017 Dani Rodríguez
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

/**
 * @file
 * @brief memory primitive functions and other structures
 *
 * This module contains function implementations designed to work with memory in kernel.
 * They are not very clean. They should probably move into new modules such as a future
 * libk or mm. Current memory management in NativeOS is very poor. The system allocates
 * memory linearly but there is no cleanup system and the current implementation is
 * prone to fragmentation due to page aligning.
 */

#include <kernel/memory.h>

/**
 * @brief First memory position after the kernel image.
 *
 * This variable is set by linker.ld. It is a bookmark set by the linker after the
 * kernel image. This is used to mark the boundaries of the actual ELF kernel image
 * in order to know when free space is available in the machine to be used by
 * the kernel as kernel heap.
 */
extern char kernel_after;

/**
 * @brief Next free memory address to be used in allocations
 *
 * This is the memory address where the next allocation will happen. Therefore, this
 * is probably also the value that will be returned as a pointer the next time an
 * allocation is made.
 */
static unsigned char* next_address = &kernel_after;

/**
 * @brief Allocate a memory region in the kernel heap.
 *
 * Invoking this function should reserve a memory region to be used as a buffer in
 * some context. The requester must tell the function how much space would it want
 * and the system should guarantee that no further calls to kmalloc will return
 * memory buffers that overlap with a previously returned memory buffer unless one of
 * them has been freed.
 *
 * The caller can request the allocated buffer to be page aligned by using the align
 * parameter. If alignment is requested, the allocated memory area will start on a new
 * empty 4 kB page, so it's guaranteed that ptr & 0xFFF == 0 for the pointer ptr. This
 * is needed for virtual memory.
 *
 * Sometimes it may be required to know the exact memory location where the buffer has
 * been allocated. If a pointer to an integer variable is provided on the phys output
 * parameter, it will be dereferenced and overwritten by the physical memory address
 * of the allocated buffer on the computer memory. To avoid this behavior simply pass
 * NULL.
 *
 * @param size how many bytes do we want to reserve.
 * @param align if true, the buffer will be aligned into the boundaries of a 4 kB page.
 * @param phys points to an integer variable where to store the memory address, or NULL.
 *
 * @return pointer to a valid buffer that has been allocated.
 */
static void *kmalloc_real(unsigned int size, int align, unsigned int *phys) {
	/*
	 * If the user wants to align the allocated area to a page do it.
	 * XXX: This implementation will introduce fragmentation if lots of small
	 * allocations are made aligned to a page.
	 */
	if (align && ((unsigned int) next_address & 0xFFF)) {
		unsigned int this_page_addr = (unsigned int) next_address & ~(0xFFF);
		next_address = (unsigned char *) this_page_addr + 0x1000;
	}

	/*
	 * If the physical address for the allocated data has been requested, place the
	 * physical address into the int variable pointed by phys. Note that this is not
	 * the same as returning a pointer. Outside this function, phys may be an int var.
	 */
	if (phys) {
		*phys = (unsigned int) next_address;
	}

    /* Allocate the buffer using a linear system. */
    unsigned char* address_to_return = next_address;
    next_address += size;
    return address_to_return;
}

void *kmalloc(unsigned int size) {
    return kmalloc_real(size, 0, 0);
}

void *kmalloc_al(unsigned int size) {
    return kmalloc_real(size, 1, 0);
}

void *kmalloc_py(unsigned int size, unsigned int *phys) {
    return kmalloc_real(size, 0, phys);
}

void *kmalloc_ap(unsigned int size, unsigned int *phys) {
    return kmalloc_real(size, 1, phys);
}
