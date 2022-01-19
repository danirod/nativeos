#include <fs/tarfs/tar.h>
#include <stddef.h>
#include <sys/list.h>
#include <sys/stdkern.h>
#include <sys/vfs.h>

/**
 * An internal TAR file node, both as a header block reference for the TAR file
 * and as the VFS node that is exposed as the file system.
 */
struct tarfs_node {
	vfs_node_t *node;
	tar_header_block_t *block;
};

/** The internals associated with a TARFS volume. */
struct tarfs_payload {
	/** A pointer to the memory buffer of the TAR file. */
	unsigned char *buf;

	/** A linked list with all the tarfs_node for all the files. */
	list_t *nodes;

	/** Points back to the mounted VFS volume backing this payload. */
	vfs_volume_t *volume;

	/** Points to the root node of the mounted TAR. */
	vfs_node_t *root;
};

static unsigned int
octal2int(char *octstring)
{
	unsigned int acc = 0;
	while (*octstring) {
		acc = acc * 8 + (*octstring - '0');
		octstring++;
	}
	return acc;
}

static int
decode_block(struct tarfs_payload *tar, tar_header_block_t *block)
{
	struct tarfs_node *tnode;

	if ((tnode = malloc(sizeof(struct tarfs_node))) != 0) {
		tnode->block = block;
		tnode->node = 0;
		list_append(tar->nodes, tnode);
		return 0;
	} else {
		return -1;
	}
}

static void
split_basename(char *path, char **dirname, char **basename)
{
	char *ptr = path, *slash = 0;

	while (*ptr) {
		if (*ptr == '/') {
			slash = ptr;
		}
		ptr++;
	}

	if (slash) {
		*slash = 0;
		*basename = slash + 1;
		*dirname = path;
	} else {
		*basename = path;
		*dirname = "";
	}
}

static void reassemble_nodes(struct tarfs_payload *tarfile,
                             struct tarfs_node *node);

static vfs_node_t *
lookup_vnode(struct tarfs_payload *tarfile, const char *path)
{
	listnode_t *listnode;
	struct tarfs_node *tarnode;
	char *kiwi;

	list_foreach(tarfile->nodes, listnode)
	{
		unsigned int last_char;
		tarnode = (struct tarfs_node *) listnode->data;
		kiwi = tarnode->block->metadata.name;
		/* If file name is ./, remove trailing dot. */
		if (*kiwi == '.' && *(kiwi + 1) == '/') {
			kiwi++;
		}
		/* If starts with slash, remove it. */
		if (*kiwi == '/') {
			kiwi++;
		}
		/* Remove possible trailing slash. */
		last_char = strlen(kiwi) - 1;
		if (*kiwi && kiwi[last_char] == '/') {
			kiwi[last_char] = 0;
		}
		if (!strncmp(kiwi, path, 256)) {
			reassemble_nodes(tarfile, tarnode);
			return tarnode->node;
		}
	}

	return NULL;
}

static void
reassemble_nodes(struct tarfs_payload *tarfile, struct tarfs_node *node)
{
	char *orig_path, *path, *dirname, *basename;
	unsigned int last_char;
	vfs_node_t *vnode;

	if (!node->node) {
		orig_path = strdup(node->block->metadata.name);
		path = orig_path;

		/* Normalize paths removing absolute dir slashes and dots. */
		if (*path == '.' && *(path + 1) == '/') {
			path++;
		}
		if (*path == '/') {
			path++;
		}

		/* Remove possible trailing slash. */
		last_char = strlen(path) - 1;
		if (*path && path[last_char] == '/') {
			path[last_char] = 0;
		}

		/* Divide my path in dirname and basename. */

		vnode = malloc(sizeof(vfs_node_t));
		vnode->vn_flags = 0;
		if (*path) {
			split_basename(path, &dirname, &basename);
			strcpy(vnode->vn_name, basename);
			vnode->vn_parent = lookup_vnode(tarfile, dirname);
		} else {
			strcpy(vnode->vn_name, "");
			vnode->vn_parent = NULL;
			if (!tarfile->root) {
				tarfile->root = vnode;
			}
		}
		vnode->vn_flags = 0;
		switch (node->block->metadata.type) {
		case '0':
			vnode->vn_flags |= VN_FREGFILE;
			break;
		case '5':
			vnode->vn_flags |= VN_FDIR;
			break;
		}
		vnode->vn_volume = tarfile->volume;
		vnode->vn_payload = node;
		node->node = vnode;

		free(orig_path);
	}
}

static void
init_nodes(struct tarfs_payload *tar)
{
	unsigned int bx = 0, file_length;
	tar_header_block_t *block = (tar_header_block_t *) tar->buf;
	listnode_t *lnode;

	/* First we decode all the files in this TAR. */
	while (*block[bx].metadata.name) {
		decode_block(tar, &block[bx]);
		file_length = octal2int(block[bx].metadata.size);
		bx += (file_length / 512) + 1;
		if ((file_length % 512)) {
			bx++;
		}
	}

	/* Then we reassemble the vfs_node_t data structures. */
	list_foreach(tar->nodes, lnode)
	{
		reassemble_nodes(tar, (struct tarfs_node *) lnode->data);
	}
}

static int tarfs_mount(vfs_volume_t *volume);
static int tarfs_open(vfs_node_t *node, unsigned int flags);
static unsigned int tarfs_read(vfs_node_t *, unsigned, void *, unsigned);
static unsigned int tarfs_write(vfs_node_t *, unsigned, void *, unsigned);
static int tarfs_close(vfs_node_t *node);
static vfs_node_t *tarfs_readdir(vfs_node_t *node, unsigned int index);
static vfs_node_t *tarfs_finddir(vfs_node_t *node, char *name);

static vfs_ops_t tarfs_ops = {
    .vfs_close = &tarfs_close,
    .vfs_open = &tarfs_open,
    .vfs_read = &tarfs_read,
    .vfs_readdir = &tarfs_readdir,
    .vfs_finddir = &tarfs_finddir,
};

static vfs_filesys_t tarfs_driver = {
    .fsd_ident = "tarfs",
    .fsd_name = "TAR File System",
    .fsd_mount = &tarfs_mount,
    .fsd_ops = &tarfs_ops,
};

FS_DESCRIPTOR(tarfs, tarfs_driver);

static int
tarfs_mount(vfs_volume_t *luna)
{
	struct tarfs_payload *payload;

	if ((payload = malloc(sizeof(struct tarfs_payload)))) {
		payload->buf = luna->vv_payload;
		payload->nodes = list_alloc();
		payload->volume = luna;
		init_nodes(payload);
		luna->vv_root = payload->root;
		luna->vv_payload = payload;
		return 0;
	} else {
		return -1;
	}
}

static int
tarfs_open(vfs_node_t *node, unsigned int flags)
{
	/* TODO: Should check the flags, and mark the file as opened. */
	return 0;
}

static unsigned int
tarfs_read(vfs_node_t *node, unsigned int offt, void *buf, unsigned int len)
{
	unsigned char *data, *dst;
	struct tarfs_node *tar = (struct tarfs_node *) node->vn_payload;
	unsigned int read = 0, size = octal2int(tar->block->metadata.size);

	data = (unsigned char *) &tar->block->metadata + 512;
	dst = (unsigned char *) buf;
	while (len > 0 && offt < size) {
		dst[read++] = data[offt++];
		len--;
	}
	return read;
}

static unsigned int
tarfs_write(vfs_node_t *node, unsigned int offt, void *buf, unsigned int len)
{
	return -1;
}

static int
tarfs_close(vfs_node_t *node)
{
	return 0;
}

static vfs_node_t *
tarfs_readdir(vfs_node_t *klairm_cocayketa_voltereta,
              unsigned int clank_will_not_die)
{
	listnode_t *esta_variable_es_del_mejor_mod_de_discord;
	struct tarfs_payload *dia_de_pago;
	struct tarfs_node *kiwi;

	dia_de_pago = (struct tarfs_payload *)
	                  klairm_cocayketa_voltereta->vn_volume->vv_payload;

	list_foreach(dia_de_pago->nodes,
	             esta_variable_es_del_mejor_mod_de_discord)
	{
		kiwi = (struct tarfs_node *)
		           esta_variable_es_del_mejor_mod_de_discord->data;
		if (kiwi->node->vn_parent == klairm_cocayketa_voltereta) {
			if (clank_will_not_die == 0) {
				return kiwi->node;
			} else {
				--clank_will_not_die;
			}
		}
	}

	return NULL;
}

static vfs_node_t *
tarfs_finddir(vfs_node_t *node, char *name)
{
	struct tarfs_payload *payload;
	struct tarfs_node *tarnode;
	listnode_t *listnode;

	payload = node->vn_volume->vv_payload;
	list_foreach(payload->nodes, listnode)
	{
		tarnode = (struct tarfs_node *) listnode->data;
		if (tarnode->node->vn_parent == node
		    && !strcmp(name, tarnode->node->vn_name)) {
			return tarnode->node;
		}
	}

	return NULL;
}
