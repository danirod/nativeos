/**
 * \file
 * \brief Intel 8253 Programmable Interrupt Timer Driver
 *
 * The PIT is a simple device that generates timed signals a specific amount
 * of times per second. One of the uses of this timed signal is to make
 * preemptive multitasking systems force a context switch if the currently
 * running task has been using the CPU for an excessive amount of time.
 */

#include <kernel/cpu/idt.h>
#include <sys/device.h>

static unsigned long next_ticks = 0;

static void pctimer_handler(struct idt_data *data);
static int pctimer_init(void);
static int pctimer_open(unsigned int flags);
static int pctimer_close(void);
static unsigned int pctimer_read(unsigned char *buf, unsigned int len);
static unsigned int pctimer_write(unsigned char *buf, unsigned int len);

static driver_t pctimer_driver = {
    .drv_name = "pctimer",
    .drv_flags = DV_FCHARDEV,
    .drv_init = &pctimer_init,
};

static device_t pctimer_device = {
    .dev_family = &pctimer_driver,
    .dev_open = &pctimer_open,
    .dev_close = &pctimer_close,
    .dev_read_chr = &pctimer_read,
};

static void
pctimer_handler(struct idt_data *data)
{
	next_ticks++;
}

static int
pctimer_init(void)
{
	idt_set_handler(0x20, &pctimer_handler);
	device_install(&pctimer_device, "pit");
	return 0;
}

static int
pctimer_open(unsigned int flags)
{
	return 0;
}

static int
pctimer_close()
{
	return 0;
}

static unsigned int
pctimer_read(unsigned char *buf, unsigned int len)
{
	static char *letters = "0123456789ABCDEF";
	unsigned int i, last, read = 0;
	unsigned long ticks = next_ticks;
	char conversion[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	for (i = 0; i < 8; i++) {
		last = ticks & 0xF;
		conversion[i] = letters[last];
		ticks >>= 4;
	}
	for (i = 7; i >= 0 && len--; --i) {
		*buf++ = conversion[i];
		read++;
	}
	return read;
}

DEVICE_DESCRIPTOR(pctimer, pctimer_driver);
