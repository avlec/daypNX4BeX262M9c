#include <stdlib.h>
#include <stdio.h>

#include "disklist.h"
#include "diskutil.h"

void read_entries_from_sector(FILE * input, disk_fat12 * disk, int16_t sector) {
  fseek(input, 512*sector, SEEK_SET); // Skip to data section!

  char * entry = (char *) malloc(sizeof(char)*32);
  if(entry == NULL) {
		fprintf(stderr, "Error allocating memory, exiting.\n");
		exit(1);
	}
  fread(entry, sizeof(char), 32, input);

  int read_count = 1;

  if(entry[0] == 0xE5)
    return;

  while(entry[0] != 0x00) {
    du_file file = new_du_file(entry, disk);
    
    // If it's valid do these things.
    if(disk_is_file_valid(&file)) {
      disk_print_file(&file);
      //disk_print_file(&file);
      
      if((file.attr & 0x10) != 0) { // is directory
        fprintf(stderr, "==================\n");
        read_entries_from_sector(input, disk, file.first_logical_cluster);
        fseek(input, (32*read_count) + (512*sector), SEEK_SET);
        fprintf(stderr, "==\n");
      }
    }
    
    fread(entry, sizeof(char), 32, input);
    read_count += 1;
  }
  return;
}

int main(int argc, char ** argv) {
  if(argc != 2) {
    fprintf(stderr, "Invoke with: $ disklist diskimage.IMA");
    exit(EXIT_FAILURE);
  }

  disk_fat12 disk = new_disk_fat12(argv[1]);

  FILE * input = fopen(argv[1], "rb");
  if(input == NULL) {
    fprintf(stderr, "Error opening file.\n");
    exit(1);
  }

  read_entries_from_sector(input, &disk, 19);

	return EXIT_SUCCESS;
}
