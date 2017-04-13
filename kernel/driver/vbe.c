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
 * File: driver/vbe.c
 * Description: VBE Vesa Video interface
 */

#include <kernel/multiboot.h>
#include <driver/vbe.h>

static vbe_control_info_t *vbe_control;
static vbe_mode_info_t *vbe_mode;

static unsigned int* vbe_framebuffer;

void vbe_init(multiboot_info_t *multiboot)
{
	vbe_control = (vbe_control_info_t *) multiboot->vbe_control_info;
	vbe_mode = (vbe_mode_info_t *) multiboot->vbe_mode_info;
	vbe_framebuffer = (unsigned char *) vbe_mode->phys_base_ptr;
}

void vbe_putpixel(unsigned short x, unsigned short y, unsigned char r, unsigned char g, unsigned char b)
{
	vbe_framebuffer[(vbe_mode->x_resolution * y) + x] = (r << 16 | g << 8 | b << 0 | 0xCC << 24);
}
