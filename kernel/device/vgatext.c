/*
 * This file is part of NativeOS
 * Copyright (C) 2015-2021 The NativeOS contributors
 * SPDX-License-Identifier:  GPL-3.0-only
 */

#include <kernel/cpu/io.h>
#include <sys/device.h>
#include <sys/stdkern.h>
#include <sys/vfs.h>

static int vgatext_init(void);

#define VGA_COLS 80
#define VGA_ROWS 25
#define VGA_SIZE (VGA_COLS * VGA_ROWS)

#define VGA_ENTRY(char, fg, bg) (char | fg << 8 | bg << 12)

#define VGA_IOR_ADDR 0x3D4
#define VGA_IOR_DATA 0x3D5

struct vgacontext {
	unsigned short *baseaddr;
	unsigned int cx, cy;
	unsigned char fg, bg;
};

static struct vgacontext context = {
    .baseaddr = (unsigned short *) 0xb8000,
    .cx = 0,
    .cy = 0,
    .fg = 7,
    .bg = 0,
};

static void
syncfbcursor()
{
	int abspos = context.cy * VGA_COLS + context.cx;
	IO_OutP(VGA_IOR_ADDR, 0xE);
	IO_OutP(VGA_IOR_DATA, (abspos >> 8) & 0xFF);
	IO_OutP(VGA_IOR_ADDR, 0xF);
	IO_OutP(VGA_IOR_DATA, abspos & 0xFF);
}

static void
enablecursor(int enabled)
{
	unsigned char in;
	if (enabled) {
		IO_OutP(VGA_IOR_ADDR, 0xA);
		in = IO_InP(VGA_IOR_DATA) & 0x1F;
		IO_OutP(VGA_IOR_DATA, in | 0xD);
		IO_OutP(VGA_IOR_ADDR, 0xB);
		in = IO_InP(VGA_IOR_DATA) & 0xC0;
		IO_OutP(VGA_IOR_DATA, in | 0xF);
	} else {
		IO_OutP(VGA_IOR_ADDR, 0xA);
		IO_OutP(VGA_IOR_DATA, 0x20);
	}
}

static inline void
copyrow(unsigned int dst, unsigned int src)
{
	unsigned short *dstptr, *srcptr;
	if (dst < VGA_ROWS && src < VGA_ROWS) {
		dstptr = context.baseaddr + (VGA_COLS * dst);
		srcptr = context.baseaddr + (VGA_COLS * src);
		memcpy(dstptr, srcptr, VGA_COLS * 2);
	}
}

static inline void
clearrow(unsigned int row)
{
	unsigned int i;
	unsigned short *rowptr = context.baseaddr + (VGA_COLS * row);
	for (i = 0; i < VGA_COLS; i++)
		rowptr[i] = VGA_ENTRY(0, 7, 0);
}

static inline void
moveline()
{
	unsigned int row;
	if (++context.cy == VGA_ROWS) {
		for (row = 1; row < VGA_ROWS; row++)
			copyrow(row - 1, row);
		clearrow(VGA_ROWS - 1);
		context.cy = VGA_ROWS - 1;
	}
}

static inline void
movecursor()
{
	if (++context.cx == VGA_COLS) {
		context.cx = 0;
		moveline();
	}
}

static void
putchar(unsigned int ch)
{
	int pos;
	switch (ch) {
	case '\n':
		moveline();
	case '\r':
		context.cx = 0;
		break;
	default:
		pos = context.cy * VGA_COLS + context.cx;
		context.baseaddr[pos] = VGA_ENTRY(ch, context.fg, context.bg);
		movecursor();
		break;
	}
	syncfbcursor();
}

static unsigned int
vgatext_write(unsigned char *buf, unsigned int len)
{
	unsigned int rem = len;
	while (rem--) {
		putchar(*buf);
		buf++;
	}
	return len;
}

static int
vgatext_open(unsigned int flags)
{
	if (flags & VO_FREAD)
		/* This is a write only device. */
		return -1;
	return 0;
}

static int
vgatext_close()
{
	return 0;
}

static driver_t vgatext_driver = {
    .dv_name = "vgacon",
    .dv_flags = DV_FCHARDEV,
    .dv_init = &vgatext_init,
};

static chardev_t vgatext_device = {
    .cd_family = &vgatext_driver,
    .cd_open = &vgatext_open,
    .cd_write = &vgatext_write,
    .cd_close = &vgatext_close,
};

static int
vgatext_init(void)
{
	unsigned int i;
	/* Clear video buffer. */
	for (i = 0; i < VGA_SIZE; i++) {
		context.baseaddr[i] = VGA_ENTRY(0, 7, 0);
	}
	syncfbcursor();
	enablecursor(1);

	device_install(&vgatext_device, "fb");
	return 0;
}

DEVICE_DESCRIPTOR(vgatext, vgatext_driver);
