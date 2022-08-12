#include <kernel/cpu/idt.h>
#include <sys/vfs.h>

struct file_descriptor {
	unsigned int offset;
	vfs_node_t *node;
};

static struct file_descriptor fds[100];

static int
find_free_descriptor()
{
	for (int monke = 0; monke < 100; monke++) {
		if (!fds[monke].node) {
			return monke;
		}
	}
	return -1;
}

static int
do_open(const char *path, int flags)
{
	int fd = find_free_descriptor();
	if (fd >= 0) {
		vfs_node_t *node = fs_resolve(path);
		if (node && fs_open(node, flags) == 0) {
			fds[fd].node = node;
			fds[fd].offset = 0;
			return fd;
		}
	}
	return -1;
}

static int
do_close(int fd)
{
	if (fds[fd].node) {
		int ret = fs_close(fds[fd].node);
		fds[fd].node = 0;
		return ret;
	}
	return -1;
}

static unsigned int
do_read(int fd, void *buf, unsigned int len)
{
	if (fds[fd].node) {
		int ret = fs_read(fds[fd].node, fds[fd].offset, buf, len);
		if (ret != (unsigned int) -1) {
			fds[fd].offset += ret;
			return ret;
		}
	}
	return (unsigned int) -1;
}

static unsigned int
do_write(int fd, void *buf, unsigned int len)
{
	if (fds[fd].node) {
		int ret = fs_write(fds[fd].node, fds[fd].offset, buf, len);
		if (ret != (unsigned int) -1) {
			fds[fd].offset += ret;
			return ret;
		}
	}
	return (unsigned int) -1;
}

void
syscall_handler(struct idt_data *data)
{
	switch (data->eax) {
	case 1: // open
		data->eax = do_open((char *) data->ebx, (int) data->ecx);
		break;
	case 2: // close
		data->eax = do_close((int) data->ebx);
		break;
	case 3: // read
		data->eax = do_read((int) data->ebx,
		                    (void *) data->ecx,
		                    (unsigned int) data->edx);
		break;
	case 4: // write
		data->eax = do_write((int) data->ebx,
		                     (void *) data->ecx,
		                     (unsigned int) data->edx);
		break;
	default:
		data->eax = -1;
		break;
	}
}

void
syscall_init(void)
{
	idt_set_handler(0x3C, &syscall_handler);
}
