/**
 * \file
 * \brief Virtual Console
 *
 * The Virtual Console is a character device that abstracts the framebuffer
 * as a character device, so that the higher level code doesn't have to do
 * manually dangerous things - such as manipulating the framebuffer directly,
 * in case the protocol ever changes from VGA/CGA to something like VESA.
 *
 * In the future, vtcon will also:
 * - Intercept calls to the keyboard driver and decode the scancodes.
 * - Transform the given string using some rules (such as ANSI escape codes).
 *
 * In the future, some of the low level I/O operations done by the vtcon
 * driver may be moved to the vgafb driver instead, once I implement ioctl.
 */

#include <kernel/cpu/io.h>
#include <sys/device.h>
#include <sys/stdkern.h>
#include <sys/vfs.h>

static int vtcon_init(void);

#define VGA_COLS 80
#define VGA_ROWS 25
#define VGA_SIZE (VGA_COLS * VGA_ROWS)

#define VGA_ENTRY(char, fg, bg) (char | fg << 8 | bg << 12)

#define VGA_IOR_ADDR 0x3D4
#define VGA_IOR_DATA 0x3D5

struct vtcontext {
	unsigned int cx, cy;
	unsigned char fg, bg;
	vfs_node_t *fb;
};

static struct vtcontext context = {
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
	unsigned char buf[VGA_COLS * 2];
	unsigned int dstofft, srcofft, read;
	if (dst < VGA_ROWS && src < VGA_ROWS) {
		srcofft = VGA_COLS * src * 2;
		dstofft = VGA_COLS * dst * 2;
		read = fs_read(context.fb, srcofft, buf, VGA_COLS * 2);
		fs_write(context.fb, dstofft, buf, read);
	}
}

static inline void
clearrow(unsigned int row)
{
	unsigned int i;
	unsigned int rowofft = VGA_COLS * row * 2;
	unsigned short value = 0x700;
	for (i = 0; i < VGA_COLS; i++)
		fs_write(context.fb, rowofft + 2 * i, &value, 2);
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
	short entry;
	switch (ch) {
	case '\n':
		moveline();
	case '\r':
		context.cx = 0;
		break;
	default:
		pos = context.cy * VGA_COLS + context.cx;
		entry = VGA_ENTRY(ch, context.fg, context.bg);
		fs_write(context.fb, 2 * pos, &entry, 2);
		movecursor();
		break;
	}
	syncfbcursor();
}

static unsigned int
vtcon_write(unsigned char *buf, unsigned int len)
{
	unsigned int rem = len;
	while (rem--) {
		putchar(*buf);
		buf++;
	}
	return len;
}

static int
vtcon_open(unsigned int flags)
{
	if (flags & VO_FREAD)
		/* This is a write only device. */
		return -1;
	if (context.fb)
		/* Device is already opened. */
		return -1;
	context.fb = fs_resolve("DEV:/fb");
	if (!context.fb || fs_open(context.fb, VO_FWRITE) < 0)
		/* Cannot open framebuffer. */
		return -1;
	return 0;
}

static int
vtcon_close()
{
	if (fs_close(context.fb) < 0)
		return -1;
	context.fb = 0;
	return 0;
}

static driver_t vtcon_driver = {
    .drv_name = "vgacon",
    .drv_flags = DV_FCHARDEV,
    .drv_init = &vtcon_init,
};

static device_t vtcon_device = {
    .dev_family = &vtcon_driver,
    .dev_open = &vtcon_open,
    .dev_write_chr = &vtcon_write,
    .dev_close = &vtcon_close,
};

static int
vtcon_init(void)
{
	syncfbcursor();
	enablecursor(1);

	device_install(&vtcon_device, "vtcon");
	return 0;
}

DEVICE_DESCRIPTOR(vtcon, vtcon_driver);
