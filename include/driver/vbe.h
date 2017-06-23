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
 * File: driver/vbe.h
 * Description: VBE 3.0 header file
 */

#ifndef DRIVER_VBE_H_
#define DRIVER_VBE_H_

#include <kernel/multiboot.h>

/**
 * Control info for VBE 3.0. This data structure is the one describe for
 * function 00h on the VBE 3.0 standard. Multiboot bootloaders that support
 * VBE will initialize by themselves. We can use this struct to parse
 * information about the VBE mode.
 */
typedef struct vbe_control_info {
	char vbe_signature[4];
	unsigned short vbe_version;
	unsigned int vbe_oem_string_ptr;
	unsigned int vbe_capabilities;
	unsigned int vbe_video_mode_ptr;
	unsigned short vbe_total_memory;
	unsigned short vbe_oem_software_rev;
	unsigned int vbe_oem_vendor_name_ptr;
	unsigned int vbe_oem_product_name_ptr;
	unsigned int vbe_oem_product_rev_ptr;
	unsigned char vbe_reserved[222];
	char vbe_oem_data[256];
} __attribute__((packed)) vbe_control_info_t;

typedef struct vbe_mode_info {
	// Basic VBE
	unsigned short mode_attributes;
	unsigned char win_a_attributes;
	unsigned char win_b_attributes;
	unsigned short win_granularity;
	unsigned short win_size;
	unsigned short win_a_segment;
	unsigned short win_b_segment;
	unsigned int win_func_ptr;
	unsigned short bytes_per_scanline;

	// VBE 1.2+
	unsigned short x_resolution;
	unsigned short y_resolution;
	unsigned char x_char_size;
	unsigned char y_char_size;
	unsigned char number_of_planes;
	unsigned char bits_per_pixel;
	unsigned char number_of_banks;
	unsigned char memory_model;
	unsigned char bank_size;
	unsigned char number_of_image_panes;
	unsigned char reserved_vbe12;

	// Direct Color Modes
	unsigned char red_mask_size;
	unsigned char red_field_position;
	unsigned char green_mask_size;
	unsigned char green_field_position;
	unsigned char blue_mask_size;
	unsigned char blue_field_position;
	unsigned char rsvd_mask_size;
	unsigned char rsvd_field_position;
	unsigned char direct_color_mode_info;

	// VBE 2.0+
	unsigned int phys_base_ptr;
	unsigned int reserved_20_1;
	unsigned short reserved_20_2;

	// VBE 3.0+
	unsigned short lin_bytes_per_scanline;
	unsigned char bnk_number_of_image_pages;
	unsigned char lin_number_of_image_pages;
	unsigned char lin_red_mask_size;
	unsigned char lin_red_field_position;
	unsigned char lin_green_mask_size;
	unsigned char lin_green_field_position;
	unsigned char lin_blue_mask_size;
	unsigned char lin_blue_field_position;
	unsigned char lin_rsvd_mask_size;
	unsigned char lin_rsvd_field_position;
	unsigned int max_pixel_clock;

	unsigned char reserved[189];
} __attribute__((packed)) vbe_mode_info_t;

void vbe_init(multiboot_info_t* mboot);

void vbe_putpixel(unsigned short x, unsigned short y, unsigned char r, unsigned char g, unsigned char b);

#endif // DRIVER_VBE_H_