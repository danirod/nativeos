#pragma once

typedef struct tar_metadata {
	char name[100];
	char mode[8];
	char uid[8];
	char gid[8];
	char size[12];
	char mktime[12];
	char checksum[8];
	char type;
	char linkname[100];
	char magic[6];
	char version[2];
	char uname[32];
	char gname[32];
	char devmajor[8];
	char devminor[8];
	char prefix[155];
} tar_metadata_t;

typedef struct tar_header_block {
	tar_metadata_t metadata;
	char reserved[12];
} tar_header_block_t;
