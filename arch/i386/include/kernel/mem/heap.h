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
#pragma once

/**
 * \file heap.h
 * \brief Kernel Heap Allocator
 *
 * The kernel heap allocator is used by the kernel to allocate objects in a
 * dynamic way during runtime.  Note that the kernel heap is only used by the
 * kernel, as the name implies.  It is used for allocate objects that are
 * required during the kernel operation, such as temporal handling of inodes or
 * other driver stuff.
 */

#include <stddef.h>

/**
 * \brief Initialises the kernel heap.
 *
 * This function must be called at least once before actually using the heap
 * for anything, because it will initialise internal data structures that the
 * heap requires for normal operation.
 */
void heap_init();

/**
 * \brief Allocate a memory region in the heap.
 *
 * This function is used by the kernel to allocate something in the kernel
 * heap. If the requested amount of memory is successfully allocated in the
 * kernel, a pointer to the first byte of such memory buffer will be returned.
 * If there is an error, such as an exhausted heap, NULL will be returned.
 *
 * \param size the amount of bytes that the kernel wants to have allocated.
 * \return either a pointer to a memory region, or NULL in case of error.
 */
void * heap_alloc(size_t size);

/**
 * \brief Free a memory region from the heap.
 *
 * This function is used by the kernel to free a memory region that previously
 * has been allocated by the heap_alloc function. The given memory region must
 * be the pointer that heap_alloc would return; i.e. the first byte of memory
 * from the memory region. After calling heap_free, the pointer should not be
 * used because it's not a safe region and may be reallocated later.
 *
 * \param ptr a pointer to the memory region to free.
 */
void heap_free(void * ptr);
