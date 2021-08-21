#include <sys/device.h>

static int null_init(void);
static unsigned int null_dev_read(unsigned char *buf, unsigned int len);
static unsigned int null_dev_write(unsigned char *buf, unsigned int len);
static unsigned int zero_dev_read(unsigned char *buf, unsigned int len);

static driver_t null_driver = {
    .dv_name = "null",
    .dv_flags = DV_FCHARDEV,
    .dv_init = &null_init,
};

static chardev_t null_chardev = {
    .cd_family = &null_driver,
    .cd_read = &null_dev_read,
    .cd_write = &null_dev_write,
};

static chardev_t zero_chardev = {
    .cd_family = &null_driver,
    .cd_read = &zero_dev_read,
    .cd_write = &null_dev_write,
};

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
	device_install(&null_chardev, "null");
	device_install(&zero_chardev, "zero");
	return 0;
}

void
null_install()
{
	null_driver.dv_init();
}
