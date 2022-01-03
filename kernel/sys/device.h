#pragma once

/**
 * \brief Declare a device descriptor
 *
 * Calling this macro providing a proper device_t structure will create a
 * marker pointer in a different section of the ELF binary image, so that
 * the device manager will be able to dynamically load it when the kernel
 * starts up.
 *
 * \param name the name to use for the exported symbol
 * \param dev the device_t structure to add to the table
 */
#define DEVICE_DESCRIPTOR(name, dev) \
	driver_t *device_descriptor_##name \
	    __attribute__((section(".text.driver"), used)) = &dev

struct device;
struct driver;

typedef int (*device_open)(unsigned int flags);
typedef unsigned int (*device_read_chr)(unsigned char *buf, unsigned int size);
typedef unsigned int (*device_write_chr)(unsigned char *buf, unsigned int size);
typedef unsigned int (*device_read_blk)(unsigned char *buf,
                                        unsigned int offt,
                                        unsigned int size);
typedef unsigned int (*device_write_blk)(unsigned char *buf,
                                         unsigned int offt,
                                         unsigned int size);
typedef int (*device_ioctl)(int iorq, void *args);
typedef int (*device_close)();

typedef struct device {
	struct driver *dev_family;
	device_open dev_open;
	device_read_blk dev_read_blk;
	device_write_blk dev_write_blk;
	device_read_chr dev_read_chr;
	device_write_chr dev_write_chr;
	device_ioctl dev_ioctl;
	device_close dev_close;
} device_t;

typedef struct driver {
	char *drv_name;
	unsigned int drv_flags;
#define DV_FCHARDEV 1
#define DV_FBLCKDEV 2
	int (*drv_init)(void);
	void (*drv_tini)(void);
} driver_t;

/**
 * \brief Init the device manager.
 *
 * This function must be called prior to attempting to mount any kind of
 * device in order to initialize the system data structures required to
 * maintain the device manager itself.
 */
void device_init(void);

/**
 * \brief Install a device.
 *
 * This function should be called by drivers when attempting to expose a
 * specific device through the device manager. A name to identify the device
 * should be given, and the device will be accessible through that name.
 *
 * The acceptable return values for this function are negative values for
 * unsuccessful installations, non-negatives otherwise. The current list of
 * status codes this function may return are:
 *
 * 0: device mounted successfully
 * -1: kernel error
 * -2: device name already exists
 *
 * \param dev the device to make accessible through the device manager.
 * \param mtname the name to use when refering to this device.
 * \return status code with the outcome of the mount operation.
 */
int device_install(device_t *dev, char *mtname);

/**
 * \brief Uninstall a device.
 *
 * This function should be called by drivers when attempting to remove a
 * previously exposed driver, using its identifier. This function will
 * silently drop requests to remove devices whose name does not match the
 * name of any mounted device.
 *
 * \param mtname the name of the device to remove from the device manager.
 */
void device_remove(char *mtname);
