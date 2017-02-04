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
static unsigned char* next_address = &kernel_after;
struct memory_block* last;

void kzero_memory(unsigned char* addr, unsigned int size)
{
    //printk("clearing %d bytes from %d\n", size, addr);
    for (int i = 0; i < size; i++) {
        *(addr + i) = NULL;
    }
}   

void* kfind_free_block(unsigned int size)
{
    struct memory_block* current = &kernel_after;
    char found = 0;

    while (current && !(current->free && size <= current->size))
        current = current->next;

    return current;
}

void kfree(unsigned char* addr)
{
    /* Remove the offset from the base pointer to the block */
    struct memory_block* block = (unsigned char*)addr - MEM_BLOCK_SIZE;
    //printk("FREEING %d, %d at %d\n", block->free, block->size, block);
    kzero_memory(addr, block->size);
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
static void* kmalloc_real(unsigned int size)
{
    if (size == 0)
        return NULL;

    struct memory_block* block;
    void* return_value = NULL; 

    if (!(block = kfind_free_block(size))) {
        //printk("ANY EXISTING FREE BLOCK FOUND\n");
        block = next_address; 

        block->free = 0;
        block->size = size;

        //printk("kmalloc block at %d\n", block);
        //printk("kmalloc %d + %d = %d\n", block, MEM_BLOCK_SIZE, (unsigned char*)block + MEM_BLOCK_SIZE);

        next_address += MEM_BLOCK_SIZE + size;
        return_value = (unsigned char*)block + MEM_BLOCK_SIZE;
    }
    //else printk("ONE EXISTING FREE BLOCK FOUND\n");

    if (last != NULL)
        last->next = block;  
    last = block;

    /* ADD OFFSET TO THE BLOCK TO PRESERVE BLOCK INFO */
    return (unsigned char*)block + MEM_BLOCK_SIZE;
}

void* kmalloc(unsigned int size)
{
    return kmalloc_real(size);
}