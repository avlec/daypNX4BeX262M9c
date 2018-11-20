#include <stdlib.h>
#include <stdio.h>

#include "diskinfo.h"

#include "diskutil.h"

const char * output_format = \
  "OS Name: %s \n"
  "Label of the disk: %s \n"
  "Total size of the disk: %d \n"
  "Free size of the disk: %d \n"
  "=============="
  "The number of files in the disk: %d \n"
  "============= \n"
  "Number of FAT copies: %d \n"
  "Sectors per FAT: %d \n";

int main(int argc, char ** argv) {
  if(argc != 2) {
    fprintf(stderr, "Invoke with: $ diskutil diskimage.IMA");
    exit(EXIT_FAILURE);
  }
  print_me();
  return EXIT_SUCCESS;
}
/*
void print_disk_info(fat12_disk * disk) {
  fat12_bootsector * bootsector = &(disk->bootsector);
  printf(output_format, bs->os_name, bs->vol_label,
	 bs->total_sector_count * bs->bytes_per_sector, bs->free_sectors * bs->bytes_per_sector, 
	 );
}
*/
