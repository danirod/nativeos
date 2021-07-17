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
 * \file alloc.c
 * \brief i386 alloc implementation for stdlib.h
 *
 * This file has the implementation of several platform dependent functions
 * present in stdlib.h that need to be defined in the kernel image.  Examples
 * of such functions are malloc or free, which allows to allocat and deallocate
 * objects from the kernel heap.
 */
#include <kernel/mem/heap.h>
#include <sys/stdkern.h>

void *
malloc (size_t size)
{
	return heap_alloc(size);
}

void
free (void * ptr)
{
	heap_free(ptr);
}
