/*
 * This file is part of NativeOS: next-gen x86 operating system
 * Copyright (C) 2015-2016 Dani Rodríguez, 2017-2018 Izan Beltrán <izanbf1803@gmail.com>
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
//#include <kernel/io.h>

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
static void* next_address = &kernel_after;
struct memory_block* last;

void* kfind_free_block(unsigned int size)
{
    struct memory_block* current = &kernel_after;
    struct memory_block* best_block = NULL; /* Block with block.size >= size and (block.size - size) closest to 0 */
    char found = 0;

    while (current) {
        if (current->free && size <= current->size && (!best_block || current->size < best_block->size)) {
            best_block = current;
            // printk("Best block: %d\n", (int)best_block);
        }
        current = current->next;
    }

    return best_block;
}

void kfree(void* addr)
{
    /* Remove the offset from the base pointer to the block */
    struct memory_block* block = addr - MEM_BLOCK_SIZE;
    // printk("FREEING block { %d, %d }  at %d\n", block->free, block->size, block);

    /* Clear memory it's unnecesary */
    // kzero_memory(addr, block->size);
    block->free = 1;
}

/** 
 * @brief Allocate a memory region in kernel mode.
 *
 * This is the real implementation of the kmalloc function. You'll usually
 * want to use the wrapper functions that provide the valid arguments
 * to the function.
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

        // printk("New block CREATED of size %d\n", size);

        next_address += MEM_BLOCK_SIZE + size;
    }
    
    if (last) 
        last->next = block; 
    last = block;
    block->free = 0;

    /*
    printk("kmalloc block at %d\n", block);
    printk("kmalloc %d + %d = %d\n", block, MEM_BLOCK_SIZE, (unsigned char*)block + MEM_BLOCK_SIZE);
    printk("kmalloc size = %d\n", size);
    */

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