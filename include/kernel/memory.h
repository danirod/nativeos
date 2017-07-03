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

#ifndef KERNEL_MEMORY_H_
#define KERNEL_MEMORY_H_ 

#define NULL 0
#define MEM_BLOCK_SIZE sizeof(struct memory_block)

/**
 * @brief Clear x bytes starting at z position of memory
 * @param addr base addres to start clearing
 * @param size number of bytes to clear
 */
#define kzero_memory(addr, size) (kmemset((addr), 0, (size)))

struct memory_block {
    char free;
    unsigned int size;
    struct memory_block* next;
};


/**
 * @brief Allocate a memory buffer in memory
 *
 * Calling this function will return a pointer to a memory buffer that can be used for
 * any purpose. The function will allocate the requested size. Because there is no
 * memory protection system at this level, it is not guaranteed the results of accessing
 * locations outside the boundaries of the requested buffer.
 *
 * @param size how many bytes to allocate.
 * @return the pointer to the memory buffer.
 */
void* kmalloc(unsigned int size);

/**
 * @brief Free memory block (after free can be reused)
 * @param addr Adress of the pointer to the block + MEM_BLOCK_SIZE
 */
void kfree(void* addr);

/**
 * @brief Find free memory blocks
 * @param size requires size for the block
 * @return pointer to block if exists else 0 (NULL)
 */
void* kfind_free_block(unsigned int size);

/**
 * @brief Set byte to a memory region
 * @param position, pointer to memory region start
 * @param byte, new value to set
 * @param nbytes, number of bytes to set starting from position
 */
void kmemset(void* poistion, char byte, unsigned int nbytes);

#endif
