#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "diskutil.h"
#include "diskput.h"

// couldn't get math library working... so needed this
int ceiling(float number) {
	int num = (int) number;
	if((number - (float)num) == 0)
		return (int) num;
	return num + 1;
}

void allocate_fat(disk_fat12 * disk, char * file_path) {
	du_file file = DU_FILE_INITIALIZER;

	// verification



	FILE * input = fopen(file_path, "rb");
	if(input == NULL) {
		fprintf(stderr, "Error opening file.\n");
		exit(1);
	}

	// Count number of sectors required to store file
	fseek(input, 0, SEEK_END);
	int sector_count = (int) ceiling((float)ftell(input)/512.0);
	fprintf(stderr, "[%ld]", ftell(input));
	rewind(input);

	fprintf(stderr, "\nrequires %d sectors\n", sector_count);

	// allocate space in disk
	int curr_sector_count = 0;
	for(int i = 0; (i < 3072) && (curr_sector_count < sector_count); ++i) { // loop over all entries.
		if(disk->fat.entries[i] != 0x00)
			continue;
		add_to_disk_file_sector_map(&file.dfsm, i);
		++curr_sector_count;
	}
	
	// write file to disk

	FILE * disk_file = fopen(disk->mount_point, "w+");
	char buffer[512];
	for(int i = 0; i < file.dfsm.map_size; ++i) {
		fseek(input, 512, SEEK_CUR);
		fseek(disk_file, (31 + file.dfsm.map[0]) * 512, SEEK_SET);
		fread(buffer, sizeof(char), 512, input);
		//fwrite(buffer, sizeof(char), 512, disk_file);
	}
	fclose(disk_file);
	fclose(input);

	// write to directory entry

	// write fat to disk
	// this invovles adding the directory entry.
}

int main(int argc, char ** argv) {
	if(argc != 3) {
		fprintf(stderr, "Invoke with: $ diskget diskimage.IMA filename.ext\n");
	}

	disk_fat12 disk = new_disk_fat12(argv[1]);

	allocate_fat(&disk, argv[2]);

	return EXIT_SUCCESS;
}
