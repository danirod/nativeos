/*
 * This file is part of NativeOS
 * Copyright (C) 2015-2020 The NativeOS contributors
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
 * \file pmm.c
 * \brief Physical Memory Manager
 *
 * The physical memory manager allocates pages.  A page is an aligned 4 KB
 * memory region (in other words, a memory block that is always 4096 bytes
 * long, whose first byte has 0x000 as their 12 least significant bits).
 *
 * The virtual memory manager depends on the physical memory manager in order
 * to allocate physical memory pages that will be put in the frames in use by
 * the virtual memory address range.
 */

#include <kernel/mem/heap.h>
#include <kernel/mem/pmm.h>
#include <machine/multiboot.h>

/**
 * \brief Frame index.
 *
 * This abstraction provides a little more context to variables that hold a
 * frame number, to make them stand out among other kind of unsigned integer
 * variables that we hold in this module.
 */
typedef unsigned int frame_idx_t;

/**
 * \brief Frame allocation bitmap
 *
 * Each bit of this bitmap represents one of the 4 KB frames the memory is
 * split by.  Bit i is set to 1 when frame i, spanning memory addresses
 * (i * 0x4000) up to (i * 0x4000 + 0x3FFF) is already taken.
 */
static unsigned int * frames_map;

/**
 * \brief Number of allocatable frames
 *
 * This is the length of the bitmap phys_frame_bitmap.  So, this variable will
 * hold the total amount of frames that exist in the memory of this machine.
 * Up to 2^20 frames can exist on a machine fully loaded with 4 GB of RAM.
 * For a system with less than 4 GB of RAM, less frames will exist.
 */
static frame_idx_t frames_count;

#define FRAME_NUMBER(memaddr) (memaddr >> 12) /* divide by 4096 */
#define PHYSICAL_ADDR(idx) (idx << 12) /* multiply by 4096 */

#define BIT_INDEX(frame_idx) (frame_idx >> 5) /* divide by 32 */
#define BIT_OFFSET(frame_idx) (frame_idx & 0x1F) /* modulus 32 */

#define OFFSET_MASK(bit) (((unsigned int) 0x1) << bit)

static inline void
frame_set (frame_idx_t idx)
{
	frames_map[BIT_INDEX(idx)] |= OFFSET_MASK(BIT_OFFSET(idx));
}

static inline unsigned int
frame_test (frame_idx_t idx)
{
	return frames_map[BIT_INDEX(idx)] & OFFSET_MASK(BIT_OFFSET(idx));
}

static inline void
frame_clear (frame_idx_t idx)
{
	frames_map[BIT_INDEX(idx)] &= ~OFFSET_MASK(BIT_OFFSET(idx));
}

/**
 * \brief Return the index of a free frame.
 *
 * Note that this implementation can be improved, because it will take more
 * time to allocate frames as the system grows the in use memory.  However,
 * this is a simple implementation that will make things kick in the meantime.
 *
 * \return either -1 if no free frames are found, or the frame index otherwise.
 */
static frame_idx_t
frame_find_free ()
{
	frame_idx_t idx, offset;
	unsigned int map;

	for (idx = 0; idx < frames_count; idx += 32) {
		if (frames_map[BIT_INDEX(idx)] != (unsigned int) -1) {
			map = frames_map[BIT_INDEX(idx)];
			for (offset = 0; offset < 32; offset++) {
				if (!(map & OFFSET_MASK(BIT_OFFSET(offset)))) {
					return idx | offset;
				}
			}
		}
	}

	return -1;
}

static void
allocate_frames ()
{
	/* mem_upper contains the size of the extended memory area.  Note that
	 * because multiboot respects the original IBM PC memory map, the
	 * extended memory starts at 1 MB, so I have to add 1024 kB to whatever
	 * I have here.  Also note that this variable is expressed in kBs.  */
	unsigned int mapsize, memsize = (multiboot_info->mem_upper + 1) << 10;

	frames_count = FRAME_NUMBER(memsize);

	mapsize = BIT_INDEX(frames_count);
	if (BIT_OFFSET(frames_count) != 0) {
		mapsize++;
	}
	frames_map = (unsigned int *) heap_alloc(mapsize);
}

/**
 * \brief Mark frames in use for a specific memory block.
 * \param mblock a memory region declared by multiboot as in use.
 */
static void
reserve_system_block (multiboot_mmap_t * mblock)
{
	physaddr_t page;
	unsigned int f, frame_offset = FRAME_NUMBER(mblock->base_addr);

	for (f = frame_offset, page = 0;
			f < frames_count && page < mblock->length;
			f++, page += 0x1000) {
		frame_set(f);
	}
}

/**
 * \brief Mark frames in use by the system as in use.
 *
 * Multiboot may report that some memory areas are reserved (hardware mappings,
 * ACPI tables, or damaged memory regions).  In such cases, it may be desired
 * to mark such frames as permanently in use so that no further pages are
 * allocated into these frames.
 */
static void
reserve_system ()
{
	multiboot_mmap_t * mblock;
	physaddr_t mmap_end;

	if (multiboot_info->flags & 0x40) {
		mblock = (multiboot_mmap_t *) multiboot_info->mmap_addr;
		mmap_end = (physaddr_t) mblock + multiboot_info->mmap_length;

		while ((physaddr_t) mblock < mmap_end) {
			/* If the block is reserved, mark the pages. */
			if (mblock->type != 1) {
				reserve_system_block(mblock);
			}

			/* Skips to the next block (forgive weird math). */
			mblock = (multiboot_mmap_t *) ((unsigned int) mblock +
				mblock->size + sizeof(mblock->size));
		}
	}
}

/**
 * \brief Mark the memory pages in use by the kernel as in use.
 *
 * This subroutine will scan the memory pages in use by the kernel binary image
 * as in use, so that the physical memory manager never tries to allocate pages
 * inside the kernel image.
 */
static void
reserve_kernel ()
{
	extern char kernel_start, kernel_after;
	physaddr_t addr;
	frame_idx_t frame;

	/* Start counting at the start of the page. */
	addr = ((physaddr_t) &kernel_start) & 0xFFFFF000;

	/* Mark pages in use by the kernel. */
	for (frame = FRAME_NUMBER(addr);
			addr < (physaddr_t) &kernel_after;
			addr += 4096, frame++) {
		frame_set(frame);
	}
}

void
pmm_init ()
{
	allocate_frames();
	reserve_system();
	reserve_kernel();
	// TODO: reserve_modules();
}

physaddr_t
pmm_alloc_page ()
{
	frame_idx_t frame = frame_find_free();
	if (frame == (unsigned int) -1) {
		return 0;
	} else {
		frame_set(frame);
		return PHYSICAL_ADDR(frame);
	}
}

void
pmm_free_page (physaddr_t page)
{
	frame_clear(FRAME_NUMBER(page));
}
