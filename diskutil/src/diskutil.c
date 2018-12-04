#include "diskutil.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <endian.h>

disk_fat12 new_disk_fat12(char * file_location) {
  FILE * file = fopen(file_location, "rb");
  if(file == NULL) {
    printf("Error opening file, exiting.\n");
    exit(1);
  }

  disk_fat12 disk;
  
  // Read in the boot sector.
  {
    char bootsector_raw_data[62];
    fread(bootsector_raw_data, sizeof(char), 62, file);
    disk.diskinfo = get_diskinfo_fat12(bootsector_raw_data);

    disk.mount_point = (char *) malloc(sizeof(char) * 128);
    memcpy(&disk.mount_point, file_location, (strlen(file_location) < 128) ? strlen(file_location)+1 : 128);
  }

  // Read in the FAT
  {
    char * primary_fat =   (char *) malloc(sizeof(char) * disk.diskinfo.bytes_per_sector * 9);
    char * secondary_fat = (char *) malloc(sizeof(char) * disk.diskinfo.bytes_per_sector * 9);
    fseek(file, disk.diskinfo.bytes_per_sector, SEEK_SET); // Skip first sector (boot)
    fread(primary_fat, sizeof(char), disk.diskinfo.bytes_per_sector * 9, file);
    fread(secondary_fat, sizeof(char), disk.diskinfo.bytes_per_sector * 9, file);
    disk.fat = get_fat_fat12(primary_fat, secondary_fat);
    free(primary_fat);
    free(secondary_fat);
  }
  fclose(file);
  return disk;
} 

int diskinfo_freesize_fat12(disk_fat12 * disk) {
  int count = 0;
  for(int i = 0; i < disk->diskinfo.total_sector_count; ++i)
    if(disk->fat.entries[i] == 0)
      ++count;
  return count;
}

int diskinfo_totalfilecount_fat12(disk_fat12 * disk) {
  // for each entrie in the root directory
  //  count++
  //  if directory, recursively count files in directories
  //    (excluding '.' and '..')
  return -1;
}