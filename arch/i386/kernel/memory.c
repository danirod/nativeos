/*
 * This file is part of NativeOS
 * Copyright (C) 2015-2018 The NativeOS contributors
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
static void* next_address = &kernel_after;
struct memory_block* last;

void* kfind_free_block(unsigned int size)
{
    struct memory_block* current = (struct memory_block *) &kernel_after;
    /* Block with block.size >= size and (block.size - size) closest to 0 */
    struct memory_block* best_block = NULL;
    char found = 0;

    while (current) {
        if (current->free && size <= current->size && (!best_block || current->size < best_block->size)) {
            best_block = current;
        }
        current = current->next;
    }

    return best_block;
}

void kfree(void* addr)
{
    /* Remove the offset from the base pointer to the block */
    struct memory_block* block = addr - MEM_BLOCK_SIZE;
    block->free = 1;
}

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
 *
 * @return pointer to a valid buffer that has been allocated.
 */
void* kmalloc(unsigned int size)
{
    if (size == 0)
        return NULL;

    struct memory_block* block = kfind_free_block(size);

    if (!block) {
        block = (struct memory_block*)next_address;
        block->size = size;
        next_address += MEM_BLOCK_SIZE + size;
    }

    if (last)
        last->next = block;
    last = block;
    block->free = 0;

    /* Add offset to the block to preserve block info */
    return (void*)block + MEM_BLOCK_SIZE;
}


void kmemset(void* position, char byte, unsigned int nbytes)
{
    void* end = position + nbytes;
    while (position < end) {
        *(char*)(position) = byte;
        position++;
    }
}
