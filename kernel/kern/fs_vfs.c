#include <sys/list.h>
#include <sys/stdkern.h>
#include <sys/vfs.h>

static list_t *vfs_volumes;
static list_t *vfs_drivers;

static int rootfs_mount(vfs_volume_t *vol);
static int rootfs_open(struct vfs_node *node, unsigned int flags);
static int rootfs_close(struct vfs_node *node);
static unsigned int rootfs_read(struct vfs_node *node,
                                unsigned int offt,
                                void *buf,
                                unsigned int len);
static vfs_node_t *rootfs_readdir(struct vfs_node *node, unsigned int index);
static vfs_node_t *rootfs_finddir(struct vfs_node *node, char *name);
static void rootfs_register_volume(vfs_volume_t *vol);
static void rootfs_unregister_volume(vfs_volume_t *vol);

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
    .vn_volume = 0,
};

static vfs_filesys_t rootfs_fs = {
    .fsd_ident = "rootfs",
    .fsd_name = "Root FS",
    .fsd_init = 0, // Manually initialised
    .fsd_mount = &rootfs_mount,
    .fsd_ops = &rootfs_ops,
};

FS_DESCRIPTOR(rootfs, rootfs_fs);

static inline vfs_volume_t *
find_mountpoint_by_name(char *mtname)
{
	listnode_t *node;
	vfs_volume_t *mountpoint;
	list_foreach(vfs_volumes, node)
	{
		mountpoint = (vfs_volume_t *) node->data;
		if (!strncmp(mountpoint->vv_name, mtname, 16)) {
			return mountpoint;
		}
	}
	return 0;
}

void
vfs_init(void)
{
	extern char fs_descriptor__start, fs_descriptor__end;
	vfs_filesys_t **fs_start, **fs_end, **fs;

	vfs_volumes = list_alloc();
	vfs_drivers = list_alloc();

	/* TODO: This shouldn't happen. */
	list_append(vfs_drivers, &rootfs_fs);

	/* TODO: This should happen after adding the drivers. */
	vfs_mount("rootfs", "ROOT", 0);

	/* Register the file system drivers. */
	fs_start = (vfs_filesys_t **) &fs_descriptor__start;
	fs_end = (vfs_filesys_t **) &fs_descriptor__end;
	for (fs = fs_start; fs < fs_end; fs++) {
		if ((*fs)->fsd_init) {
			(*fs)->fsd_init();
		}
		list_append(vfs_drivers, *fs);
	}
}

vfs_filesys_t *
get_driver_by_name(char *name)
{
	vfs_filesys_t *fs;
	listnode_t *node;

	list_foreach(vfs_drivers, node)
	{
		fs = (vfs_filesys_t *) node->data;
		if (!strcmp(name, fs->fsd_ident)) {
			return fs;
		}
	}
	return NULL;
}

int
vfs_mount(char *driver, char *name, void *argp)
{
	vfs_volume_t *volume;
	vfs_filesys_t *family;

	if (find_mountpoint_by_name(name) != 0) {
		return -1;
	}
	if ((family = get_driver_by_name(driver)) == 0) {
		return -1;
	}
	if ((volume = (vfs_volume_t *) malloc(sizeof(vfs_volume_t))) == 0) {
		return -1;
	}
	volume->vv_name = strdup(name);
	volume->vv_payload = argp;
	volume->vv_family = family;
	volume->vv_root = NULL;
	if (family->fsd_mount(volume) < 0) {
		free(volume->vv_name);
		free(volume);
		return -1;
	}
	list_append(vfs_volumes, volume);
	rootfs_register_volume(volume);
	return 0;
}

int
vfs_umount(char *mountname)
{
	vfs_volume_t *vol = find_mountpoint_by_name(mountname);
	if (vol) {
		rootfs_unregister_volume(vol);
		list_delete(vfs_volumes, vol);
		free(vol->vv_name);
		free(vol);
		return 0;
	}
	return -1;
}

vfs_node_t *
vfs_get_volume(char *mountname)
{
	vfs_volume_t *mtpoint = find_mountpoint_by_name(mountname);
	if (mtpoint)
		return mtpoint->vv_root;
	else
		return 0;
}

static list_t *rootfs_nodes;

static void
rootfs_register_volume(vfs_volume_t *vol)
{
	vfs_node_t *mati_stop_using_haskell;

	mati_stop_using_haskell = (vfs_node_t *) malloc(sizeof(vfs_node_t));
	strcpy(mati_stop_using_haskell->vn_name, vol->vv_name);
	mati_stop_using_haskell->vn_flags = VN_FREGFILE;
	mati_stop_using_haskell->vn_volume = rootfs_root.vn_volume;
	mati_stop_using_haskell->vn_parent = &rootfs_root;
	mati_stop_using_haskell->vn_payload = vol;
	list_append(rootfs_nodes, mati_stop_using_haskell);
}

static void
rootfs_unregister_volume(vfs_volume_t *vol)
{
	listnode_t *node;
	vfs_node_t *vfs_node;

	list_foreach(rootfs_nodes, node)
	{
		vfs_node = (vfs_node_t *) node->data;
		if (!strcmp(vfs_node->vn_name, vol->vv_name)) {
			list_delete(rootfs_nodes, vfs_node);
			return;
		}
	}
}

static int
rootfs_mount(vfs_volume_t *vol)
{
	if (rootfs_nodes) {
		return -1;
	}

	rootfs_nodes = list_alloc();
	rootfs_root.vn_volume = vol;
	vol->vv_root = &rootfs_root;
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
	vfs_node_t *vfs_node;
	if (index >= list_count(rootfs_nodes)) {
		return 0;
	}
	return list_at(rootfs_nodes, index);
}

static vfs_node_t *
rootfs_finddir(struct vfs_node *node, char *name)
{
	listnode_t *l_node;
	vfs_node_t *vfs_node;

	list_foreach(rootfs_nodes, l_node)
	{
		vfs_node = (vfs_node_t *) l_node->data;
		if (!strcmp(vfs_node->vn_name, name)) {
			return vfs_node;
		}
	}
	return 0;
}
