#include <kernel/fs.h>

static int
nine_fd_open (struct vfs_node * node, void * argp)
{
	return 0;
}

static int
nine_fd_close (struct vfs_node * node)
{
	return 0;
}

static size_t
nine_fd_read (struct vfs_node * node, unsigned char * buffer,
		unsigned int offset, size_t length)
{
	char * ptr = (char *) buffer + offset;
	size_t count = 0;
	while (length--) {
		ptr[count++] = '9';
	}
	return count;
}

static size_t
nine_fd_write (struct vfs_node * node, unsigned char * buffer,
		unsigned int offset, size_t length)
{
	return length;
}

static struct vfs_node nine_fd = {
	.type = VFS_TYPE_CHARDEV,
	.open = &nine_fd_open,
	.read = &nine_fd_read,
	.write = &nine_fd_write,
	.close = &nine_fd_close
};
