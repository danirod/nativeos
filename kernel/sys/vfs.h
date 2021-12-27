#pragma once

struct vfs_node;

#define VO_FREAD 0x0001 /**< Open the file to read from it. */
#define VO_FWRITE 0x0002 /**< Open the file to write into it. */
#define VO_FBINARY 0x004 /**< Open the file in binary mode. */

#define VN_FREGFILE 0x1
#define VN_FDIR 0x2
#define VN_FCHARDEV 0x3
#define VN_FBLOCKDEV 0x4

typedef int (*vfs_open)(struct vfs_node *node, unsigned int flags);
typedef unsigned int (*vfs_read)(struct vfs_node *node,
                                 unsigned int offset,
                                 void *buf,
                                 unsigned int len);
typedef unsigned int (*vfs_write)(struct vfs_node *node,
                                  unsigned int offset,
                                  void *buf,
                                  unsigned int len);
typedef int (*vfs_close)(struct vfs_node *node);
typedef struct vfs_node *(*vfs_readdir)(struct vfs_node *node,
                                        unsigned int index);
typedef struct vfs_node *(*vfs_finddir)(struct vfs_node *node, char *nodename);

typedef struct vfs_node {
	char vn_name[64];
	unsigned int vn_flags;
	struct vfs_node *vn_parent;
	void *vn_payload;
	vfs_open vn_open;
	vfs_read vn_read;
	vfs_write vn_write;
	vfs_close vn_close;
	vfs_readdir vn_readdir;
	vfs_finddir vn_finddir;
} vfs_node_t;

void vfs_init(void);
int vfs_mount(vfs_node_t *node, char *mountname);
int vfs_umount(char *mountname);

vfs_node_t *vfs_get_volume(char *mountname);

/**
 * @brief Given a path, locate the node in the mounted filesystems
 * @param path the path to the node we want to find
 * @return a pointer to the found node or NULL if not found
 */
vfs_node_t *fs_resolve(const char *path);

int fs_open(vfs_node_t *node, unsigned int flags);
unsigned int
fs_read(vfs_node_t *node, unsigned int offset, void *buf, unsigned int len);
unsigned int
fs_write(vfs_node_t *node, unsigned int offset, void *buf, unsigned int len);
int fs_close(vfs_node_t *node);
vfs_node_t *fs_readdir(vfs_node_t *node, unsigned int index);
vfs_node_t *fs_finddir(vfs_node_t *node, char *name);
