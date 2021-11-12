#include <kernel/cpu/idt.h>
#include <kernel/cpu/io.h>
#include <sys/device.h>
#include <sys/ringbuf.h>

static int keyboard_init(void);
static int keyboard_open(unsigned int flags);
static int keyboard_close(void);
static unsigned int keyboard_read(unsigned char *buf, unsigned int len);

static driver_t keyboard_driver = {
    .dv_name = "keyboard",
    .dv_flags = DV_FCHARDEV,
    .dv_init = &keyboard_init,
};

static chardev_t keyboard_chardev = {
    .cd_family = &keyboard_driver,
    .cd_open = &keyboard_open,
    .cd_read = &keyboard_read,
    .cd_close = &keyboard_close,
};

static ringbuf_t *keyboard_rbuf;

static int
keyboard_open(unsigned int flags)
{
	return 0;
}

static int
keyboard_close(void)
{
	return 0;
}

static unsigned int
keyboard_read(unsigned char *buf, unsigned int len)
{
	unsigned int total = 0;
	unsigned char scancode;

	while (total <= len && ringbuf_test_ready(keyboard_rbuf)) {
		scancode = ringbuf_read(keyboard_rbuf);
		*buf++ = scancode;
		total++;
	}

	return total;
}

static void
keyboard_key_handler(struct idt_data *idt)
{
	int scancode = IO_InP(0x60);
	ringbuf_write(keyboard_rbuf, (char) scancode);
}

static int
keyboard_init(void)
{
	keyboard_rbuf = ringbuf_alloc(1024);
	idt_set_handler(0x21, &keyboard_key_handler);
	device_install(&keyboard_chardev, "kbd");
	return 0;
}

void
keyboard_install()
{
	keyboard_driver.dv_init();
}
