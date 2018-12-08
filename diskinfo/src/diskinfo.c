#include <stdlib.h>
#include <stdio.h>

#include "diskinfo.h"

const char * output_format =
  "OS Name: %s \n"
  "Label of the disk: %s \n"
  "Total size of the disk: %d bytes\n"
  "Free size of the disk: %d bytes\n"
  "============== \n"
  "The number of files in the disk: %d \n"
  "============= \n"
  "Number of FAT copies: %d \n"
  "Sectors per FAT: %d \n";

int count_entries_from_sector(FILE * input, disk_fat12 * disk, int16_t sector) {
  int file_count = 0;
  
  fseek(input, 512*sector, SEEK_SET); // Skip to data section!
  char * entry = (char *) malloc(sizeof(char)*32);
  fread(entry, sizeof(char), 32, input);
  int read_count = 1;

  if(entry[0] == 0xE5)
    return file_count;

  while(entry[0] != 0x00) {
    du_file file = new_du_file(entry, disk);
    
    // If it's valid do these things.
    if(disk_is_file_valid(&file)) {
      ++file_count;
      if((file.attr & 0x10) != 0) { // is directory
        file_count += count_entries_from_sector(input, disk, file.first_logical_cluster);
        fseek(input, (32*read_count) + (512*sector), SEEK_SET);
      }      
    }
    
    fread(entry, sizeof(char), 32, input);
    read_count += 1;
  }
  return file_count;
}

int diskinfo_freesize_fat12(disk_fat12 * disk) {
  int count = 0;
  for(int i = 0; i < disk->diskinfo.total_sector_count; ++i)
    if(disk->fat.entries[i] == 0)
      ++count;
  return count;
}

int main(int argc, char ** argv) {
  if(argc != 2) {
    fprintf(stderr, "Invoke with: $ diskinfo diskimage.IMA");
    exit(EXIT_FAILURE);
  }
  
  disk_fat12 disk = new_disk_fat12(argv[1]);

  FILE * input = fopen(argv[1], "rb");

  printf(output_format, disk.diskinfo.os_name, disk.diskinfo.vol_label,
	  disk.diskinfo.bytes_per_sector * disk.diskinfo.total_sector_count,
	  diskinfo_freesize_fat12(&disk) * 512, count_entries_from_sector(input, &disk, 19),
	  disk.diskinfo.num_fats, disk.diskinfo.sectors_per_fat);

  return EXIT_SUCCESS;
}
