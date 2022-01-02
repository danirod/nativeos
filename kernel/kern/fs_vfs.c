#include <sys/list.h>
#include <sys/stdkern.h>
#include <sys/vfs.h>

typedef struct mountpoint {
	char vm_mountname[16];
	vfs_node_t *vm_root;
	vfs_node_t *vm_rootfs_entry;
} mountpoint_t;
static list_t *vfs_volumes;

static int rootfs_open(struct vfs_node *node, unsigned int flags);
static int rootfs_close(struct vfs_node *node);
static unsigned int rootfs_read(struct vfs_node *node,
                                unsigned int offt,
                                void *buf,
                                unsigned int len);
static vfs_node_t *rootfs_readdir(struct vfs_node *node, unsigned int index);
static vfs_node_t *rootfs_finddir(struct vfs_node *node, char *name);
static vfs_node_t *rootfs_create_entry(mountpoint_t *vmtp);

static vfs_ops_t rootfs_ops = {
    .vfs_open = rootfs_open,
    .vfs_read = rootfs_read,
    .vfs_close = rootfs_close,
    .vfs_readdir = rootfs_readdir,
    .vfs_finddir = rootfs_finddir,
};

/**
 * \brief Root File System
 *
 * The Root File System is a special file system that is mounted when the
 * VFS system is being initialised. It allows to expose the VFS mountpoints
 * using the VFS itself.
 */
static vfs_node_t rootfs_root = {
    .vn_name = {0},
    .vn_flags = VN_FDIR,
    .vn_ops = &rootfs_ops,
};

static inline mountpoint_t *
find_mountpoint_by_name(char *mtname)
{
	listnode_t *node;
	mountpoint_t *mountpoint;
	list_foreach(vfs_volumes, node)
	{
		mountpoint = (mountpoint_t *) node->data;
		if (!strncmp(mountpoint->vm_mountname, mtname, 16)) {
			return mountpoint;
		}
	}
	return 0;
}

void
vfs_init(void)
{
	vfs_volumes = list_alloc();
	vfs_mount(&rootfs_root, "ROOT");
}

int
vfs_mount(vfs_node_t *node, char *mountname)
{
	mountpoint_t *vmtp;

	if ((vmtp = find_mountpoint_by_name(mountname)) != 0)
		return -2;
	if ((vmtp = (mountpoint_t *) malloc(sizeof(mountpoint_t))) == 0)
		return -1;
	strncpy(vmtp->vm_mountname, mountname, 16);
	vmtp->vm_root = node;
	if ((vmtp->vm_rootfs_entry = rootfs_create_entry(vmtp)) == 0) {
		/* Mount error. */
		free(vmtp);
		return -1;
	}
	list_append(vfs_volumes, vmtp);
	return 0;
}

static vfs_node_t *
rootfs_create_entry(mountpoint_t *vmtp)
{
	vfs_node_t *rootfs_entry = 0;

	if ((rootfs_entry = malloc(sizeof(vfs_node_t))) != 0) {
		strncpy(rootfs_entry->vn_name, vmtp->vm_mountname, 64);
		rootfs_entry->vn_flags = VN_FREGFILE;
		rootfs_entry->vn_payload = vmtp;
		rootfs_entry->vn_parent = &rootfs_root;
		rootfs_entry->vn_ops = &rootfs_ops;
	}

	return rootfs_entry;
}

int
vfs_umount(char *mountname)
{
	mountpoint_t *vmtp = find_mountpoint_by_name(mountname);
	if (vmtp) {
		list_delete(vfs_volumes, vmtp);
		free(vmtp->vm_rootfs_entry);
		free(vmtp);
		return 0;
	}
	return -1;
}

vfs_node_t *
vfs_get_volume(char *mountname)
{
	mountpoint_t *mtpoint = find_mountpoint_by_name(mountname);
	if (mtpoint)
		return mtpoint->vm_root;
	else
		return 0;
}

static int
rootfs_open(struct vfs_node *node, unsigned int flags)
{
	if ((flags & VO_FREAD) == 0 || (flags & VO_FWRITE) != 0) {
		return -1;
	}
	return 0;
}

static int
rootfs_close(struct vfs_node *node)
{
	return 0;
}

static unsigned int
rootfs_read(struct vfs_node *node,
            unsigned int offt,
            void *buf,
            unsigned int len)
{
	return 0;
}

static vfs_node_t *
rootfs_readdir(struct vfs_node *node, unsigned int index)
{
	mountpoint_t *mtpoint;
	if (index >= list_count(vfs_volumes)) {
		return 0;
	}
	if ((mtpoint = list_at(vfs_volumes, index)) != 0) {
		return mtpoint->vm_rootfs_entry;
	} else {
		return 0;
	}
}

static vfs_node_t *
rootfs_finddir(struct vfs_node *node, char *name)
{
	mountpoint_t *mtpoint = find_mountpoint_by_name(name);
	if (mtpoint) {
		return mtpoint->vm_rootfs_entry;
	} else {
		return 0;
	}
}
