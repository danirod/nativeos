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
 * File: paging.h
 * Description: pagination and virtualization
 */

#ifndef KERNEL_PAGING_H_
#define KERNEL_PAGING_H_

/** 
 * @brief Data structure that represents an entry in the page table.
 *
 * This data structure must be 32 bit and uses bit maps because we want
 * to access particular bit flags in the structure. Also it is so complex
 * that it would be unappealing to access it via bitmasks.
 */
typedef struct page_entry {
    unsigned int present: 1; /**< Is this memory page loaded in a frame? */
    unsigned int rw: 1; /**< Can I write on this memory page? */
    unsigned int user: 1; /**< Is this page accessible from user-mode? */
    unsigned int accessed: 1; /**< Is this page being accessed? */
    unsigned int dirty: 1; /**< Has this page been modified? */
    unsigned int unused: 7; /**< Several bytes reserved that aren't used. */
    unsigned int frame: 20; /**< Number of phys. frame this page points to. */
} __attribute__((packed)) page_entry_t;

/** 
 * @brief Data structure that represents a page table full of page entries.
 *
 * This data structure must be 4096 bytes long so that it can be loaded in
 * a complete frame / its size must be an entire page.
 */
typedef struct page_table {
    page_entry_t entries[1024];
} __attribute__((packed)) page_table_t;

/** 
 * @brief Set up the frame allocation system.
 *
 * This function has to be called before initializing Virtual Memory.
 * Creates a bitmap in memory for every possible frame in the physical
 * memory, specifying which frames are free and which frames not.
 * The amount of memory the computer has, has to be given in KB. This is
 * not hard, since GRUB already reports the memory size in KB.
 * 
 * @param memory_size  the amount of memory in this computer, in KB.
 */
void frames_init(unsigned int memory_size);

/** 
 * @brief Mark a frame as being used.
 * 
 * @param frame_no number of frame that needs to be marked.
 */
void frame_set(unsigned int frame_no);

/** 
 * @brief Is a frame being used?
 * 
 * @param frame_no number of frame that we want to see if is used or not.
 * 
 * @return 1 if the frame is used and 0 if not.
 */
int frame_is_set(unsigned int frame_no);

/**
 * @brief Get the frame number for the first frame that is free.
 * 
 * Please, note that this method is at the moment not optimized.
 * The system will work worse as more frames are reserved because
 * it has no optimizations. I repeat: NO optimizations.
 * XXX: Fix that.
 * 
 * @return first free frame in the bitmap.
 */
int frame_first_free();

/** 
 * @brief Mark a frame as clean.
 * 
 * @param frame_no number of frame that is set free.
 */
void frame_free(unsigned int frame_no);

#endif // KERNEL_PAGING_H_
