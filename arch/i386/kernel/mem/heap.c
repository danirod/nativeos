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
 * \file heap.c
 * \brief x86 kernel heap
 *
 * This file implements a heap allocator designed to be used by the kernel.
 * The heap is a big chunk of memory where an application may allocate objects
 * dynamically.
 *
 * The current kernel heap implementation is a linked list of memory regions.
 * Each memory region has a control block as header (including fields such as
 * the size of the memory region or whether it's allocated or not), and a data
 * region as body.  The data region is the space where the application may
 * write to or read from.
 *
 * Whenever a new buffer is allocated, a free memory region is selected, and
 * it's split in two segments.  One of the segments has the requested amount of
 * bytes on its data region; the remaining part of the original memory region
 * is marked with a new control block in order to have it free for future use.
 *
 * Whenever a buffer is freed, the memory segment is marked as free.  Further
 * allocations may use this memory buffer again, even split it again.  This is
 * a fragmentation source because there may be a lot of small regions marked as
 * active, with small free gaps in between that canont be used to allocate
 * bigger objects.
 *
 * In order to reduce fragmentation, every time a memory region is deallocated,
 * the bounds of the memory region are checked to see if there are other free
 * regions adjacent to the one that's being freed.  If two consecutive free
 * regions are found, those regions are merged into one consecutive and big
 * memory region.
 */

#include <kernel/mem/heap.h>
#include <kernel/cpu/spinlock.h>

/** Magic number that indicates that a heap control block follows.  */
#define HEAP_MAGIC_HEAD 0x51514949

/** Magic value that indicates that the current memory block is free.  */
#define HEAP_MAGIC_FREE 0x40404040

/** Magic value that indicates that the current memory block is allocated.  */
#define HEAP_MAGIC_USED 0xEFEFEFEF

/** Points to a memory address related to the heap.  */
typedef unsigned char * HEAP_ADDR;

/** Used to indicate the size of a memory region in the heap.  */
typedef size_t HEAP_SIZE;

/**
 * \brief Header control block
 *
 * The header control block is a structure that's present at the beginning of a
 * memory segment in the heap.  It's used to mark metadata about the memory
 * region itself, such as whether the memory region is free and can be
 * allocated, or if it's currently in use.
 */
typedef struct heap_block {
	int magic, status;
	HEAP_ADDR next, prev;
	HEAP_SIZE size;
} heap_block_t;

/* This points to the root control block of the heap once it has been
 * initialised.  Control blocks are connected through a linked list.  The
 * allocator will traverse the linked list when looking for blocks marked as
 * free.  */
static volatile heap_block_t * heap_root;

/* Defined in ldscript.  This symbol is located at the bottom of the kernel
 * heap.  Heap allocations should always start at a memory address that is
 * equal or greater than the memory address of this symbol.  */
extern volatile const unsigned char heap_bottom;

/* Defined in ldscript.  This symbol is located at the top of the kernel heap.
 * Heap allocations should never touch any memory address whose value is equal
 * or greater than the memory address of this symbol.  */
extern volatile const unsigned char heap_top;

/* Forbids multiple processors of allocating memory at the same time.  */
static spinlock_t heap_allocator_spinlock;

/**
 * \brief Attempt to merge the given heap control blocks.
 * \param head the first memory block to merge.
 * \param tail the second memory block to merge.
 * \return pointer to the merged block, or NULL if no merge was made.
 */
static inline heap_block_t *
heap_merge (heap_block_t * head, heap_block_t * tail)
{
	HEAP_ADDR head_addr = (HEAP_ADDR) head;
	HEAP_ADDR tail_addr = (HEAP_ADDR) tail;
	heap_block_t * bound;

	if (head->magic != HEAP_MAGIC_HEAD || head->magic != tail->magic) {
		/* Both headers must be valid.  */
		return 0;
	}
	if (head->status != HEAP_MAGIC_FREE || head->status != tail->status) {
		/* Both headers must be marked as free.  */
		return 0;
	}
	if (head->next != (HEAP_ADDR) tail || tail->prev != (HEAP_ADDR) head) {
		/* Memory regions must be linked.  */
		return 0;
	}
	if (head_addr + sizeof(heap_block_t) + head->size != tail_addr) {
		/* Memory regions must be consecutive. */
		return 0;
	}

	/* Unlink tail from chain. */
	if (tail->next) {
		bound = (heap_block_t *) tail->next;
		bound->prev = (HEAP_ADDR) head;
	}
	head->next = tail->next;

	/* Assignate the space for the control block and buffer to head. */
	head->size += (sizeof(heap_block_t) + tail->size);

	return head;
}

void
heap_init (void)
{
	HEAP_SIZE total;
	HEAP_ADDR top, bottom;

	/* Extract the heap size.  Just remember that heap_top and heap_bottom
	 * are symbols defined in the linkerscript, therefore we should only
	 * access the memory address, not the values themselves.  Also, top is
	 * at the end of the heap because memory maps go bottom to up.  */
	bottom = (HEAP_ADDR) &heap_bottom;
	top = (HEAP_ADDR) &heap_top;
	total = (HEAP_SIZE) (top - bottom);

	/* Create the root control block for the heap.  */
	heap_root = (heap_block_t *) bottom;
	heap_root->magic = HEAP_MAGIC_HEAD;
	heap_root->status = HEAP_MAGIC_FREE;
	heap_root->size = total - sizeof(heap_block_t);
	heap_root->prev = 0;
	heap_root->next = 0;
}

void *
heap_alloc (size_t size)
{
	heap_block_t * root, * block, * next_block;
	HEAP_ADDR blockptr, buffer, after_buffer;

	root = (heap_block_t *) heap_root;
	buffer = 0;

	/* Make sure we are the only allocator in the neighbourhood.  */
	spinlock_lock(&heap_allocator_spinlock, "");

	for (block = root; block; block = (heap_block_t *) block->next) {
		blockptr = (HEAP_ADDR) block;

		if (block->magic != HEAP_MAGIC_HEAD) {
			/* This is not a heap control block!  Ackchyually,
			 * yeah, nothing guarantees us that this is a rogue
			 * control block with valid magic numbers.  */
			goto _cleanup;
		}

		if (block->status != HEAP_MAGIC_FREE) {
			/* Block cannot be used because it's allocated.  */
			continue;
		}

		if (block->size < size) {
			/* Block is not big enough for this allocation.  */
			continue;
		}

		/* Okay, so if we are here, we can allocate.  */
		block->status = HEAP_MAGIC_USED;
		buffer = (HEAP_ADDR) (blockptr + sizeof(heap_block_t));

		/* Early return if there is not enough space for split.  */
		if (block->size <= (size + sizeof(heap_block_t) + 1)) {
			goto _cleanup; /* No.  */
		}

		after_buffer = (HEAP_ADDR) (buffer + size);
		next_block = (heap_block_t *) after_buffer;

		/* Mark the bounds of the buffer as a new block.  */
		next_block->magic = HEAP_MAGIC_HEAD;
		next_block->status = HEAP_MAGIC_FREE;
		next_block->size = block->size - size - sizeof(heap_block_t);
		next_block->prev = (HEAP_ADDR) block;
		next_block->next = block->next;

		/* Shrink the current block.  */
		block->size = size;
		block->next = (HEAP_ADDR) next_block;

		/* No iterations required anymore.  */
		break;
	}
_cleanup:
	/* Make sure to unlock the spinlock or the system will collapse.  */
	spinlock_release(&heap_allocator_spinlock);
	return buffer;
}

void
heap_free (void * ptr)
{
	heap_block_t * bufheader, * nextblock, * prevblock;

	/* Lock before doing anything useful.  */
	spinlock_lock(&heap_allocator_spinlock, "");

	/* If this is an allocated buffer, it should have a header.  */
	bufheader = (heap_block_t *) (ptr - sizeof(heap_block_t));
	if (bufheader->magic != HEAP_MAGIC_HEAD) {
		/* Hey wait a second!  */
		goto _cleanup;
	}

	/* It should be used.  */
	if (bufheader->status != HEAP_MAGIC_USED) {
		goto _cleanup;
	}

	/* Mark the block as free.  */
	bufheader->status = HEAP_MAGIC_FREE;

	/* Test for merge. */
	if (bufheader->next) {
		heap_merge(bufheader, (heap_block_t *) bufheader->next);
	}
	if (bufheader->prev) {
		heap_merge((heap_block_t *) bufheader->prev, bufheader);
	}
_cleanup:
	/* Make sure to unlock the spinlock or things will collapse.  */
	spinlock_release(&heap_allocator_spinlock);
}
