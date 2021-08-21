/*
 * This file is part of NativeOS
 * Copyright (C) 2015-2021 The NativeOS contributors
 * SPDX-License-Identifier:  GPL-3.0-only
 */

#include <kernel/cpu/io.h>
#include <sys/device.h>
#include <sys/stdkern.h>

static int vgatext_init(void);

#define VGA_COLS 80
#define VGA_ROWS 25
#define VGA_SIZE (VGA_COLS * VGA_ROWS)

#define VGA_ENTRY(char, fg, bg) (char | fg << 8 | bg << 12)

#define VGA_IOR_ADDR 0x3D4
#define VGA_IOR_DATA 0x3D5
#define VGA_CMD_HICUR 0xE
#define VGA_CMD_LOCUR 0xF

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
	IO_OutP(VGA_IOR_ADDR, VGA_CMD_HICUR);
	IO_OutP(VGA_IOR_DATA, (abspos >> 8) & 0xFF);
	IO_OutP(VGA_IOR_ADDR, VGA_CMD_LOCUR);
	IO_OutP(VGA_IOR_DATA, abspos & 0xFF);
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
	unsigned short *rowptr = context.baseaddr + (VGA_COLS * row);
	memset(rowptr, 0, VGA_COLS * 2);
}

static inline void
moveline()
{
	unsigned int row;
	if (++context.cy == VGA_ROWS) {
		for (row = 1; row < VGA_ROWS; row++)
			copyrow(row - 1, row);
		clearrow(VGA_ROWS - 1);
		context.cy--;
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

static driver_t vgatext_driver = {
    .dv_name = "vgacon",
    .dv_flags = DV_FCHARDEV,
    .dv_init = &vgatext_init,
};

static chardev_t vgatext_device = {
    .cd_family = &vgatext_driver,
    .cd_write = &vgatext_write,
};

static int
vgatext_init(void)
{
	/* Clear video buffer. */
	memset(context.baseaddr, 0, VGA_SIZE);
	syncfbcursor();

	device_install(&vgatext_device, "fb");
	return 0;
}

void
vgatext_install()
{
	vgatext_driver.dv_init();
}
