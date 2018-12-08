#include "diskutil.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <endian.h>

disk_fat12 new_disk_fat12(char * file_location) {
  disk_fat12 disk;

  disk.mount_point = (char *) malloc(sizeof(char) * 128);
  memcpy(disk.mount_point, file_location, (strlen(file_location) < 128) ? strlen(file_location) : 128);
  
  FILE * input = fopen(disk.mount_point, "rb");
  if(input == NULL) {
    printf("Error opening file, exiting.\n");
    exit(1);
  }
  
  // Read in the boot sector.
  {
    char bootsector_raw_data[62];
    fread(bootsector_raw_data, sizeof(char), 62, input);
    disk.diskinfo = get_diskinfo_fat12(bootsector_raw_data);
  }
  
  // Read in the FAT
  {
    char * primary_fat =   (char *) malloc(sizeof(char) * disk.diskinfo.bytes_per_sector * 9);
    char * secondary_fat = (char *) malloc(sizeof(char) * disk.diskinfo.bytes_per_sector * 9);
    
    fseek(input, disk.diskinfo.bytes_per_sector, SEEK_SET); // Skip first sector (boot)
    
    fread(primary_fat, sizeof(char), disk.diskinfo.bytes_per_sector * 9, input);
    fread(secondary_fat, sizeof(char), disk.diskinfo.bytes_per_sector * 9, input);
    
    disk.fat = get_fat_fat12(primary_fat, secondary_fat);
    
    free(primary_fat);
    free(secondary_fat);
  }
  
  fclose(input);

  return disk;
}