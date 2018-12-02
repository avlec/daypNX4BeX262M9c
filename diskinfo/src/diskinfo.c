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

int main(int argc, char ** argv) {
  if(argc != 2) {
    fprintf(stderr, "Invoke with: $ diskutil diskimage.IMA");
    exit(EXIT_FAILURE);
  }
  
  disk_fat12 disk = new_disk_fat12(argv[1]);

  printf(output_format, disk.diskinfo.os_name, disk.diskinfo.vol_label,
	 disk.diskinfo.bytes_per_sector * disk.diskinfo.total_sector_count,
	 diskinfo_freesize_fat12(&disk), diskinfo_totalfilecount_fat12(&disk),
	 disk.diskinfo.num_fats, disk.diskinfo.sectors_per_fat);

  return EXIT_SUCCESS;
}
