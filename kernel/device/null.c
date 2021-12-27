#include <sys/device.h>

static int null_init(void);
static int null_dev_open(unsigned int flags);
static unsigned int null_dev_read(unsigned char *buf, unsigned int len);
static unsigned int null_dev_write(unsigned char *buf, unsigned int len);
static unsigned int zero_dev_read(unsigned char *buf, unsigned int len);
static int null_dev_close();

static driver_t null_driver = {
    .drv_name = "null",
    .drv_flags = DV_FCHARDEV,
    .drv_init = &null_init,
};

static device_t null_device = {
    .dev_family = &null_driver,
    .dev_open = &null_dev_open,
    .dev_read_chr = &null_dev_read,
    .dev_write_chr = &null_dev_write,
    .dev_close = &null_dev_close,
};

static device_t zero_device = {
    .dev_family = &null_driver,
    .dev_open = &null_dev_open,
    .dev_read_chr = &zero_dev_read,
    .dev_write_chr = &null_dev_write,
    .dev_close = &null_dev_close,
};

static int
null_dev_open(unsigned int flags)
{
	return 0;
}

static int
null_dev_close()
{
	return 0;
}

static unsigned int
null_dev_read(unsigned char *buf, unsigned int len)
{
	/* Read zero bytes. */
	return 0;
}

static unsigned int
null_dev_write(unsigned char *buf, unsigned int len)
{
	/* Should act as if it could write every byte. */
	return len;
}

static unsigned int
zero_dev_read(unsigned char *buf, unsigned int len)
{
	unsigned i;
	for (i = 0; i < len; i++)
		buf[i] = 0;
	return len;
}

static int
null_init(void)
{
	device_install(&null_device, "null");
	device_install(&zero_device, "zero");
	return 0;
}

DEVICE_DESCRIPTOR(null, null_driver);
