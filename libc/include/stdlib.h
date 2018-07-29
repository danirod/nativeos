#pragma once

#include <stddef.h>

/**
 * @brief Allocate some memory buffer in the heap.
 * @param size the amount of bytes to allocate in the heap.
 * @return a pointer to the allocated buffer or NULL if there was an error.
 */
void * malloc (size_t size);

/**
 * @brief Deallocate a memory buffer from the heap.
 * @param ptr a pointer to the memory region to be deallocated from heap.
 */
void free (void * ptr);
