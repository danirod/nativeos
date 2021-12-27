/**
 * \file
 * \brief VGA Framebuffer
 *
 * The VGA Framebuffer is a block device that exposes the memory region
 * reserved to the framebuffer, so that other code can use VFS operations
 * to get or change the contents of the screen.
 *
 * Applications will usually not use this file directly, but rather delegate
 * changing the contents of the screen to the virtual console device, which
 * also will make additional transformations, such as changing attributes.
 */

#include <sys/device.h>
#include <sys/stdkern.h>
#include <sys/vfs.h>

#define VGA_BASE 0xB8000
#define VGA_COLS 80
#define VGA_ROWS 25
#define VGA_CHARS (VGA_COLS * VGA_ROWS)
#define VGA_SIZE (VGA_CHARS * 2)
#define VGA_LIMIT (VGA_BASE + VGA_SIZE)

static int vgafb_init(void);
static int vgafb_open(unsigned int flags);
static int vgafb_close(void);
static unsigned int
vgafb_read(unsigned char *buf, unsigned int off, unsigned int len);
static unsigned int
vgafb_write(unsigned char *buf, unsigned int off, unsigned int len);

static driver_t vgafb_driver = {
    .drv_name = "vgafb",
    .drv_flags = DV_FBLCKDEV,
    .drv_init = &vgafb_init,
};

static device_t vgafb_device = {
    .dev_family = &vgafb_driver,
    .dev_open = &vgafb_open,
    .dev_close = &vgafb_close,
    .dev_read_blk = &vgafb_read,
    .dev_write_blk = &vgafb_write,
};

static int
vgafb_init(void)
{
	unsigned int i;
	unsigned short *vga = (unsigned short *) VGA_BASE;

	if (device_install(&vgafb_device, "fb") < 0) {
		return -1;
	}
	for (i = 0; i < VGA_CHARS; i++)
		vga[i] = 0x700;
	return 0;
}

static int
vgafb_open(unsigned int flags)
{
	return 0;
}

static int
vgafb_close(void)
{
	return 0;
}

static unsigned int
vgafb_read(unsigned char *buf, unsigned int offt, unsigned int len)
{
	unsigned char *start, *end, *ptr;
	unsigned int read_bytes = 0;

	if (offt < VGA_SIZE) {
		start = (unsigned char *) VGA_BASE + offt;
		end = start + len;
		if (end >= (unsigned char *) VGA_LIMIT) {
			end = (unsigned char *) VGA_LIMIT;
		}

		for (ptr = start; ptr < end; ptr++) {
			*buf++ = *ptr;
			read_bytes++;
		}
	}

	return read_bytes;
}

static unsigned int
vgafb_write(unsigned char *buf, unsigned int offt, unsigned int len)
{
	unsigned char *start, *end, *ptr;
	unsigned int write_bytes = 0;

	if (offt < VGA_SIZE) {
		start = (unsigned char *) VGA_BASE + offt;
		end = start + len;
		if (end >= (unsigned char *) VGA_LIMIT) {
			end = (unsigned char *) VGA_LIMIT;
		}

		for (ptr = start; ptr < end; ptr++) {
			*ptr = *buf++;
			write_bytes++;
		}
	}

	return write_bytes;
}

DEVICE_DESCRIPTOR(vgafb, vgafb_driver);