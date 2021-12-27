#include <sys/vfs.h>

int
fs_open(vfs_node_t *node, unsigned int flags)
{
	if (node->vn_open) {
		return node->vn_open(node, flags);
	}
	return -1;
}

int
fs_close(vfs_node_t *node)
{
	if (node->vn_close) {
		return node->vn_close(node);
	}
	return -1;
}

unsigned int
fs_read(vfs_node_t *node, unsigned int offt, void *buf, unsigned int len)
{
	if (node->vn_read) {
		return node->vn_read(node, offt, buf, len);
	}
	return -1;
}

unsigned int
fs_write(vfs_node_t *node, unsigned int offt, void *buf, unsigned int len)
{
	if (node->vn_write) {
		return node->vn_write(node, offt, buf, len);
	}
	return -1;
}

int
fs_ioctl(vfs_node_t *node, int iorq, void *args)
{
	if (node->vn_ioctl) {
		return node->vn_ioctl(node, iorq, args);
	}
	return -1;
}

vfs_node_t *
fs_readdir(vfs_node_t *node, unsigned int index)
{
	if (node->vn_readdir) {
		return node->vn_readdir(node, index);
	}
	return 0;
}

vfs_node_t *
fs_finddir(vfs_node_t *node, char *name)
{
	if (node->vn_finddir) {
		return node->vn_finddir(node, name);
	}
	return 0;
}
