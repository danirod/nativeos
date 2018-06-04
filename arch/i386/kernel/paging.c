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

#include <kernel/kernel.h>
#include <kernel/memory.h>
#include <kernel/multiboot.h>
#include <kernel/paging.h>

/**
 * This is the bitmap that will reference every possible frame in this
 * machine. The bit I tells the state of the frame I. Either can be 1
 * (frame is used) or 0 (frame is free).
 */
static unsigned char* frames_bitmap;

/**
 * How many bytes has the bitmap. This is used when traversing the tree.
 */
static unsigned int bitmap_size;

#define FRAME_INDEX(f) (f / 8)
#define FRAME_OFFSET(f) (f % 8)
#define FRAME_NUMBER(i, o) (8 * i + o)

/**
 * @brief Calculate the amount of memory that this computer has.
 * @param multiboot_info Multiboot structure (it knows everything).
 * @return the amount of KB this PC has or -1 if we cannot know.
 */
static unsigned int count_memory(multiboot_info_t *multiboot_info)
{
    if (multiboot_info->flags & 0x01) {
        unsigned int start = multiboot_info->mem_lower;
        /* Multiboot reports the ending area minus 1 MB. Increment it. */
        unsigned int end = multiboot_info->mem_upper + 1024;
        return end - start;
    } else {
        return 0; /* We cannot know. */
    }
}

void frame_set(unsigned int frame_no)
{
    unsigned int index = FRAME_INDEX(frame_no);
    unsigned int offset = FRAME_OFFSET(frame_no);
    frames_bitmap[index] |= (1 << offset);
}

void frame_free(unsigned int frame_no)
{
    unsigned int index = FRAME_INDEX(frame_no);
    unsigned int offset = FRAME_OFFSET(frame_no);
    frames_bitmap[index] &= ~(1 << offset);
}

int frame_is_set(unsigned int frame_no)
{
    unsigned int index = FRAME_INDEX(frame_no);
    unsigned int offset = FRAME_OFFSET(frame_no);
    return (frames_bitmap[index] & (1 << offset)) != 0;
}

int frame_first_free()
{
    /* XXX: Highly unoptimized. Will be slower as less frames are free. */
    int index, offset;
    for (index = 0; index < bitmap_size; index++) {
        /* Check if there is a free frame in this position. */
        for (offset = 0; offset < 8; offset++) {
            if (!(frames_bitmap[index] & (1 << offset))) {
                /* Hey, we found one! */
                return FRAME_NUMBER(index, offset);
            }
        }
    }
    return -1; /* There are no frames in this system. */
}

void frames_init(multiboot_info_t* multiboot_ptr)
{
	/* How many frames? A frame is 4 KB. */
	unsigned int memory_size = count_memory(multiboot_ptr);
	unsigned int frames = memory_size / 4;

	/* Reserve the bitmap structure. */
	bitmap_size = frames / 8;
	frames_bitmap = kmalloc(bitmap_size);
}
