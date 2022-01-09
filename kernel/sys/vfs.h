#pragma once

struct vfs_node;
struct vfs_filesys;

#define VO_FREAD 0x0001 /**< Open the file to read from it. */
#define VO_FWRITE 0x0002 /**< Open the file to write into it. */
#define VO_FBINARY 0x004 /**< Open the file in binary mode. */

#define VN_FREGFILE 0x1
#define VN_FDIR 0x2
#define VN_FCHARDEV 0x3
#define VN_FBLOCKDEV 0x4

/**
 * \brief Virtual File System Operations
 *
 * A file system implementation will implement some of the hooks in this
 * data structure. If the operating system receives a hook request on behalf
 * of a specific node, it will call the hook implementation as long as the
 * node operations table implements the given function.
 */
typedef struct vfs_ops {
	/**
	 * \brief Open a VFS node for further access.
	 *
	 * This hook can only be implemented by regular files and devices.
	 *
	 * The file system implementation should allocate a way for the system
	 * to send further VFS requests to the node, such as reading or
	 * writing.
	 *
	 * \param node the VFS node that will be opened
	 * \param flags a flags mask with the node open mode.
	 * \return status code: zero on success, non-zero on failure.
	 */
	int (*vfs_open)(struct vfs_node *node, unsigned int flags);

	/**
	 * \brief Read from a VFS node.
	 *
	 * This hook can only be implemented by regular files and devices.
	 *
	 * A file system implementation that uses this hook should copy into
	 * the given buffer \p buf up to \p len bytes from the underlying
	 * container mapped to the VFS node, starting at offset \p offt.
	 *
	 * \param node the VFS node that will be read
	 * \param offt the offset to apply within the container
	 * \param buf the buffer where the read bytes should be copied
	 * \param len the number of bytes to copy into the buffer
	 * \return the actual number of bytes that were copied to the buffer
	 */
	unsigned int (*vfs_read)(struct vfs_node *node,
	                         unsigned int offt,
	                         void *buf,
	                         unsigned int len);

	/**
	 * \brief Write to a VFS node.
	 *
	 * This hook can only be implemented by regular files and devices.
	 *
	 * A file system implementation that uses this hook should copy from
	 * the given buffer \p buf up to \p len bytes into the underlying
	 * container mapped to the VFS node, starting at offset \p offt.
	 *
	 * \param node the VFS node that will be written
	 * \param offt the offset to apply within the container
	 * \param buf the buffer where the bytes to copy are located
	 * \param len the number of bytes to copy into the container
	 * \return the actual number of bytes that were copied from the buffer
	 */
	unsigned int (*vfs_write)(struct vfs_node *node,
	                          unsigned int offt,
	                          void *buf,
	                          unsigned int len);

	/**
	 * \brief IO Control for VFS nodes.
	 *
	 * This hook can only be implemented by devices.
	 *
	 * ioctl is a special hack that has been in use for a very long time
	 * to execute side requests into device driver files. It can only be
	 * used by devices, and the semantics of the request depend in the
	 * device driver itself.
	 *
	 * \param node the VFS node where the request will be sent
	 * \param iorq the request code number to send to the device
	 * \param args additional iorq-dependent parameter payload
	 * \return the outcome of the ioctl request as defined by the driver
	 */
	int (*vfs_ioctl)(struct vfs_node *node, int iorq, void *args);

	/**
	 * \brief Close an VFS node
	 *
	 * This hook can only be implemented by regular files and devices.
	 *
	 * This hook is used by the file system implementation to mark the
	 * node as closed. The file system implementation should assume that
	 * after this hook is called, no further requests to the given VFS
	 * node will be received until another open request is received.
	 *
	 * \param node the VFS node to close.
	 * \return zero if successful, non-zero if not successful
	 */
	int (*vfs_close)(struct vfs_node *node);

	/**
	 * \brief Read a directory entry
	 *
	 * This hook can only be implemented by directories.
	 *
	 * This hook is used to traverse the VFS node representation of a
	 * directory in order to look for child VFS nodes that are contained
	 * inside the directory. To iterate, an \p index number is given,
	 * which should return a proper node if it points to a valid node
	 * inside this directory. Otherwise, it should return NULL to mark
	 * that the index is out of the bounds of the directory children.
	 *
	 * \todo This is not a directory entry, this is an vfs_node.
	 * \param node the VFS node to iterate
	 * \param index the index number of the child VFS node to find.
	 * \return a pointer to the child VFS node or NULL.
	 */
	struct vfs_node *(*vfs_readdir)(struct vfs_node *node,
	                                unsigned int index);

	/**
	 * \brief Find a directory entry
	 *
	 * This hook can only be implemented by directories.
	 *
	 * This hook is used to locate a specific node inside the given
	 * VFS node by its name. It is a way to lookup a specific entry
	 * given the name of the entry. It should return a pointer to the
	 * VFS node if the given directory contains a child VFS node
	 * that matches this name. Otherwise, it should return NULL.
	 *
	 * \todo This is not a directory entry, this is an vfs_node.
	 * \param node the VFS node to lookup into
	 * \param nodename the name of the child node to lookup
	 * \return a pointer to the child VFS ndoe or NULL.
	 */
	struct vfs_node *(*vfs_finddir)(struct vfs_node *node, char *nodename);
} vfs_ops_t;

typedef struct vfs_node {
	char vn_name[64];
	unsigned int vn_flags;
	struct vfs_node *vn_parent;
	struct vfs_volume *vn_volume;
	void *vn_payload;
} vfs_node_t;

typedef struct vfs_volume {
	char *vv_name;
	struct vfs_filesys *vv_family;
	struct vfs_node *vv_root;
	void *vv_payload;
} vfs_volume_t;

typedef struct vfs_filesys {
	/** The identifier name of the file system driver. */
	char *fsd_ident;

	/** The public name of the file system driver. */
	char *fsd_name;

	/** A hook to tell the FS Driver it is being initialised. */
	void (*fsd_init)(void);

	/** A hook to tell the FS Driver it is being destructed. */
	void (*fsd_tini)(void);

	/** A hook used to tell the FS Driver that a volume is mounted. */
	int (*fsd_mount)(vfs_volume_t *vol);

	/** A pointer to the operations table for this FS Driver. */
	vfs_ops_t *fsd_ops;
} vfs_filesys_t;

/**
 * \brief Get the file system operations table for a VFS node.
 * \param node the file system node to retrieve the operations table.
 * \return the operations table for the given node.
 */
#define NODE_OPS(node) \
	((node && node->vn_volume && node->vn_volume \
	  && node->vn_volume->vv_family) \
	     ? node->vn_volume->vv_family->fsd_ops \
	     : 0)

#define FS_DESCRIPTOR(name, fs) \
	vfs_filesys_t *fs_descriptor_##name \
	    __attribute__((section(".text.fs"), used)) = &fs

void vfs_init(void);

int vfs_mount(char *driver, char *name, void *argp);
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
int fs_ioctl(vfs_node_t *node, int iorq, void *args);
int fs_close(vfs_node_t *node);
vfs_node_t *fs_readdir(vfs_node_t *node, unsigned int index);
vfs_node_t *fs_finddir(vfs_node_t *node, char *name);
