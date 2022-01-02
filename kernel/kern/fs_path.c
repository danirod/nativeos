#include <sys/stdkern.h>
#include <sys/vfs.h>

/**
 * Recursively locate the given path in the given directory VFS node. If
 * the path points to a file name, such as "hello.txt", it will lookup for
 * the given file in the directory. Otherwise, it will lookup for nested
 * directories if present. In case of error or if the file is not found,
 * this function returns NULL.
 */
static vfs_node_t *
fs_follow_path(vfs_node_t *root, char *path)
{
	char *out;
	vfs_node_t *node;

	/* Get the next path component and locate it in the file system. */
	out = strsep(&path, "/");
	node = root->vn_finddir(root, out);

	/* This was the last element in the chain, so return it. */
	if (!path) {
		return node;
	}

	/* There are more components, but this is not a directory. */
	if ((node->vn_flags & VN_FDIR) == 0) {
		return 0;
	}

	/* This is a directory, and node is a proper directory. */
	return fs_follow_path(node, path);
}

vfs_node_t *
fs_resolve(const char *path)
{
	vfs_node_t *descriptor = 0;
	char *strsep_orig, *strsep_in, *strsep_out;

	/*
	 * We do not want strsep to modify the given path parameter, so we
	 * duplicate it. Also, we inmediately keep an _orig reference to do
	 * the free. strsep will be modifying _in value.
	 */
	strsep_orig = strdup(path);
	strsep_in = strsep_orig;

	/* Strip the volume separator and try to lookup the proper volume. */
	strsep_out = strsep(&strsep_in, ":");
	if (!strsep_in) {
		goto defer;
	}
	descriptor = vfs_get_volume(strsep_out);

	/*
	 * There are some early fail preconditions here: if the volume is not
	 * found, it is an invalid path. If the path does not contain a slash
	 * right after the /, it is also a path error.
	 */
	if (!descriptor || *strsep_in != '/') {
		goto defer;
	}

	/* Then start processing the path to look for the file in the device. */
	strsep_in++;
	if (*strsep_in != 0) {
		descriptor = fs_follow_path(descriptor, strsep_in);
	}

defer:
	free(strsep_orig);
	return descriptor;
}
