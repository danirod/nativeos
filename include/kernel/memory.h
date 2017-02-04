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
 * File: kernel/memory.h
 * Description: memory primitives and structures
 */

#ifndef KERNEL_MEMORY_H_
#define KERNEL_MEMORY_H_ 

#define NULL 0
#define MEM_BLOCK_SIZE sizeof(struct memory_block)

struct memory_block {
    char free;
    unsigned int size;
    struct memory_block* next;
};

/** 
 * @brief Allocate a memory buffer (the traditional way).
 * @param size how many bytes to allocate.
 * @return the pointer to the memory buffer.
 */
void* kmalloc(unsigned int size);

/**
 * @brief Free memory block (after free can be reused)
 * @param addr Adress of the pointer to the block + MEM_BLOCK_SIZE
 */
void kfree(unsigned char* addr);

/**
 * @brief Clear x bytes starting at z position of memory
 * @param addr base addres to start clearing
 * @param size number of bytes to clear
 */
void kzero_memory(unsigned char* addr, unsigned int size);

/**
 * @brief Find free memory blocks
 * @param size requires size for the block
 * @return pointer to block if exists else 0 (NULL)
 */
void* kfind_free_block(unsigned int size);

#endif