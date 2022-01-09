# This file defines macros that make testing the VFS a little bit easier.

# dump-fs-drivers
#  Prints to the debugger console information about the list of available
#  file system drivers that can be used when mounting volumes and other
#  VFS entities.
define dump-fs-drivers
	set $node = vfs_drivers->head
	while $node != 0
		set $data = (vfs_filesys_t *) $node->data
		printf "* filesys: %s (%s)\n", $data->fsd_ident, $data->fsd_name
		set $node = $node->next
	end
end

# dump-fs-volumes
#  Prints to the debugger console information about the list of currently
#  mounted volumes, showing the name of the volume, and the file system
#  driver backing the volume.
define dump-fs-volumes
	set $node = vfs_volumes->head
	while $node != 0
		set $data = (vfs_volume_t *) $node->data
		printf "* %s (%s)\n", $data->vv_name, $data->vv_family->fsd_ident
		set $node = $node->next
	end
end
