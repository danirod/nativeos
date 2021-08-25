#pragma once

struct chardev;
struct driver;

typedef int (*chardev_open)(unsigned int flags);
typedef unsigned int (*chardev_read)(unsigned char *buf, unsigned int size);
typedef unsigned int (*chardev_write)(unsigned char *buf, unsigned int size);
typedef int (*chardev_close)();

typedef struct chardev {
	struct driver *cd_family;
	chardev_open cd_open;
	chardev_read cd_read;
	chardev_write cd_write;
	chardev_close cd_close;
} chardev_t;

typedef struct driver {
	char *dv_name;
	unsigned int dv_flags;
#define DV_FCHARDEV 1
#define DV_FBLCKDEV 2
	int (*dv_init)(void);
	void (*dv_tini)(void);
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
int device_install(chardev_t *dev, char *mtname);

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
