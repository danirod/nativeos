#include <sys/vfs.h>

int
fs_open(vfs_node_t *node, unsigned int flags)
{
	if (node && node->vn_ops && node->vn_ops->vfs_open) {
		switch (node->vn_flags) {
		case VN_FREGFILE:
		case VN_FCHARDEV:
		case VN_FBLOCKDEV:
			return node->vn_ops->vfs_open(node, flags);
		}
	}
	return -1;
}

int
fs_close(vfs_node_t *node)
{
	if (node && node->vn_ops && node->vn_ops->vfs_close) {
		switch (node->vn_flags) {
		case VN_FREGFILE:
		case VN_FCHARDEV:
		case VN_FBLOCKDEV:
			return node->vn_ops->vfs_close(node);
		}
	}
	return -1;
}

unsigned int
fs_read(vfs_node_t *node, unsigned int offt, void *buf, unsigned int len)
{
	if (node && node->vn_ops && node->vn_ops->vfs_read) {
		switch (node->vn_flags) {
		case VN_FREGFILE:
		case VN_FCHARDEV:
		case VN_FBLOCKDEV:
			return node->vn_ops->vfs_read(node, offt, buf, len);
		}
	}
	return -1;
}

unsigned int
fs_write(vfs_node_t *node, unsigned int offt, void *buf, unsigned int len)
{
	if (node && node->vn_ops && node->vn_ops->vfs_write) {
		switch (node->vn_flags) {
		case VN_FREGFILE:
		case VN_FCHARDEV:
		case VN_FBLOCKDEV:
			return node->vn_ops->vfs_write(node, offt, buf, len);
		}
	}
	return -1;
}

int
fs_ioctl(vfs_node_t *node, int iorq, void *args)
{
	if (node && node->vn_ops && node->vn_ops->vfs_ioctl) {
		switch (node->vn_flags) {
		case VN_FCHARDEV:
		case VN_FBLOCKDEV:
			return node->vn_ops->vfs_ioctl(node, iorq, args);
		}
	}
	return -1;
}

vfs_node_t *
fs_readdir(vfs_node_t *node, unsigned int index)
{
	if (node && node->vn_ops && node->vn_ops->vfs_readdir) {
		if (node->vn_flags == VN_FDIR) {
			return node->vn_ops->vfs_readdir(node, index);
		}
	}
	return 0;
}

vfs_node_t *
fs_finddir(vfs_node_t *node, char *name)
{
	if (node && node->vn_ops && node->vn_ops->vfs_finddir) {
		if (node->vn_flags == VN_FDIR) {
			return node->vn_ops->vfs_finddir(node, name);
		}
	}
	return 0;
}
