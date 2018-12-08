#include <stdlib.h>
#include <stdio.h>

#include "diskutil.h"
#include "diskput.h"

void allocate_fat(disk_fat12 * disk) {
	char buffer[512];
	FILE * input = fopen(disk->mount_point, "rb");
	
	int sector_count = 0;

	// Count number of sectors required to store file
	while(!feof(input)) {
		fread(buffer, sizeof(char), 512, input);
		++sector_count;
	}

	// allocate space in disk
	disk_file_sector_map dfsm = DISK_FILE_SECTOR_MAP_INIT;
	int curr_sector_count = 0;
	for(int i = 0; (i < 3072) && (curr_sector_count <= sector_count); ++i) { // loop over all entries.
		if(disk->fat.entries[i] != 0x00)
			continue;
		add_to_disk_file_sector_map(&dfsm, i);
		++curr_sector_count;
	}
	
	// write file to disk

	// write fat to disk
}

int main(int argc, char ** argv) {
	if(argc != 3) {
		fprintf(stderr, "Invoke with: $ diskget diskimage.IMA filename.ext\n");
	}

	disk_fat12 disk = new_disk_fat12(argv[1]);

	allocate_fat(&disk);

	return EXIT_SUCCESS;
}
