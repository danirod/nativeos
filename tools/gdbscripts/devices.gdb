# This file defines macros that make testing the devmgr a little bit easier.

# dump-devices
#  Prints to the console output the list of currently available devices,
#  including information about the device family. This list is looked up
#  using information exposed from the DEVFS file system.
define dump-dev-devices
set $node = devmgr_list->head
	while $node != 0
		set $devfile = (vfs_node_t *) $node->data
		set $device = (device_t *) $devfile->vn_payload
		set $family = (driver_t *) $device->dev_family

		printf "Device: %s. ", $devfile->vn_name
		printf "(Family: %s", $family->drv_name
		if ($family->drv_flags & 3) == 1
			printf ", char"
		else
			if ($family->drv_flags & 3) == 2
				printf ", block"
			end
		end
		printf ")\n"

		set $node = $node->next
	end
end
