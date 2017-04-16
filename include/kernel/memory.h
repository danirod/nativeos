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
void *kmalloc(unsigned int size);

/**
 * @brief Allocate a memory buffer aligned to the bounds of a new page.
 *
 * This function behaves similar to kmalloc, but it will align the buffer into the
 * boundary of a page. This is similar to how valloc works on the C standard library.
 * Please note that given the current implementation of the memory management system
 * for this kernel, this will introduce a lot of overhead and memory fragmentation.
 *
 * @param size how many bytes to allocate.
 * @return the pointer to the memory buffer.
 */
void *kmalloc_al(unsigned int size);

/**
 * @brief Allocate a memory buffer.
 *
 * This function works the same as kmalloc, but there is an additional pointer parameter
 * that will be dereferenced by kmalloc and overwritten with the value of the physical
 * memory address where the buffer has been allocated into memory.
 *
 * @param size how many bytes to allocate.
 * @param phys the memory address the buffer starts in.
 * @return a pointer to the memory buffer.
 */
void *kmalloc_py(unsigned int size, unsigned int *phys);

/**
 * @brief Allocate a memory buffer aligned to the bounds of a new page.
 *
 * This function works the same as kmalloc_al, but there is an additional pointer
 * parameter that will be dereferenced by kmalloc and overwritten with the value of the
 * physical memory address where the buffer has been allocated into memory.
 *
 * @param size how many bytes to allocate.
 * @param phys the memory address the buffer starts in.
 * @return a pointer to the memory buffer.
 */
void *kmalloc_ap(unsigned int size, unsigned int *phys);

#endif // KERNEL_MEMORY_H_
