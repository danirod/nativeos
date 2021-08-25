#include <sys/list.h>
#include <sys/stdkern.h>
#include <sys/vfs.h>

typedef struct mountpoint {
	char vm_mountname[16];
	vfs_node_t *vm_root;
} mountpoint_t;
static list_t *vfs_volumes;

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
	list_append(vfs_volumes, vmtp);
	return 0;
}

int
vfs_umount(char *mountname)
{
	mountpoint_t *vmtp = find_mountpoint_by_name(mountname);
	if (vmtp) {
		list_delete(vfs_volumes, vmtp);
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
