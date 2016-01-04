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
 * File: kernel/memory.h
 * Description: memory primitives and structures
 */

#ifndef KERNEL_MEMORY_H_
#define KERNEL_MEMORY_H_

/**
 * @brief Dynamically allocate a memory chunk.
 * 
 * This function will return a memory region that is dynamically
 * allocated. It is guaranteed that this function will not return
 * twice the same memory region.
 * 
 * @param siz  how many bytes to allocate.
 */
void *kmalloc(unsigned int siz);

#endif // KERNEL_MEMORY_H_
