#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <endian.h>

#include "diskutil.h"

/* This function is responsible for reading the raw data from
 * the character string gathered from the input file and storing
 * it in an intermediate format to process.
 */
bootsector_raw_fat12 get_bootsector_fat12(char * bootsector_raw_data) {
  bootsector_raw_fat12 bootsector_raw;
  // THIS mess
  memcpy(&bootsector_raw.endianness,           &bootsector_raw_data[0],   3);
  memcpy(&bootsector_raw.os_name,              &bootsector_raw_data[3],   8);
  memcpy(&bootsector_raw.bytes_per_sector,     &bootsector_raw_data[11],  2);
  memcpy(&bootsector_raw.sectors_per_cluster,  &bootsector_raw_data[13],  1);
  memcpy(&bootsector_raw.num_reserved_sectors, &bootsector_raw_data[14],  2);
  memcpy(&bootsector_raw.num_fats,             &bootsector_raw_data[16],  1);
  memcpy(&bootsector_raw.max_rootdir_entries,  &bootsector_raw_data[17],  2);
  memcpy(&bootsector_raw.total_sector_count,   &bootsector_raw_data[19],  2);
  memcpy(&bootsector_raw.ignore_2,             &bootsector_raw_data[21],  1);
  memcpy(&bootsector_raw.sectors_per_fat,      &bootsector_raw_data[22],  2);
  memcpy(&bootsector_raw.sectors_per_track,    &bootsector_raw_data[24],  2);
  memcpy(&bootsector_raw.num_heads,            &bootsector_raw_data[26],  2);
  memcpy(&bootsector_raw.ignore_3,             &bootsector_raw_data[28],  4);
  memcpy(&bootsector_raw.ignore_4,             &bootsector_raw_data[32],  4);
  memcpy(&bootsector_raw.ignore_5,             &bootsector_raw_data[36],  2);
  memcpy(&bootsector_raw.boot_sig,             &bootsector_raw_data[38],  1);
  memcpy(&bootsector_raw.vol_id,               &bootsector_raw_data[39],  4);
  memcpy(&bootsector_raw.vol_label,            &bootsector_raw_data[43], 11);
  memcpy(&bootsector_raw.fs_type,              &bootsector_raw_data[54],  8);

  return bootsector_raw;
}

diskinfo_fat12 get_diskinfo_fat12(char * bootsector_raw_data) {
  bootsector_raw_fat12 bootsector_raw = get_bootsector_fat12(bootsector_raw_data);
  diskinfo_fat12 diskinfo;
  // THIS mess
  diskinfo.endianness =           (bootsector_raw.endianness[2] == '<') ? 'L' : 'B';
  memcpy(&diskinfo.os_name,       &bootsector_raw.os_name, 8);
  diskinfo.os_name[8] = '\0';
  diskinfo.bytes_per_sector =     le16toh(bootsector_raw.bytes_per_sector);
  diskinfo.sectors_per_cluster =  bootsector_raw.sectors_per_cluster;
  diskinfo.num_reserved_sectors = le32toh(bootsector_raw.num_reserved_sectors);
  diskinfo.num_fats =             bootsector_raw.num_fats;  
  diskinfo.max_rootdir_entries =  le32toh(bootsector_raw.max_rootdir_entries);
  diskinfo.total_sector_count =   le32toh(bootsector_raw.total_sector_count);
  diskinfo.sectors_per_fat =      le32toh(bootsector_raw.sectors_per_fat);
  diskinfo.sectors_per_track =    le32toh(bootsector_raw.sectors_per_track);
  diskinfo.num_heads =            le32toh(bootsector_raw.num_heads);
  diskinfo.boot_sig =             bootsector_raw.boot_sig;
  memcpy(&diskinfo.vol_id,        &bootsector_raw.vol_id, 4);
  diskinfo.vol_id[4] =            '\0'; // Pad String
  memcpy(&diskinfo.vol_label,     &bootsector_raw.vol_label, 11);
  diskinfo.vol_label[11] =        '\0'; // Pad String
  memcpy(&diskinfo.fs_type,       &bootsector_raw.fs_type, 8);
  diskinfo.fs_type[8] =           '\0'; // pad string

  return diskinfo;
}

fat_fat12 get_fat_fat12(char * first_raw_fat_data, char * second_raw_fat_data) {
  fat_fat12 fat;
  
  // read 24 bits at a time
  int8_t byte_1 = 0;
  int8_t byte_2 = 0;
  int8_t byte_3 = 0;


  // i=1 -> IGNORE FIRST 2
  // Read 1536 double entries (sets of 3 bytes)
  // from first fat (3072 total)
  for(int i = 1; i < 1536; i++) {
    byte_1 = first_raw_fat_data[i * 3];
    byte_2 = first_raw_fat_data[(i * 3) + 1];
    byte_3 = first_raw_fat_data[(i * 3) + 2];

    // Process Little Endianness
    // NOTE <<<THIS MIGHT NOT WORK>>>
    fat.entries[i*2] =     ((byte_2 & 0xF0) >> 4 << 8) | ((byte_1 & 0x0F) >> 0 << 4) | ((byte_1 & 0xF0) >> 4 << 0);
    fat.entries[i*2 + 1] = ((byte_3 * 0x0F) >> 0 << 8) | ((byte_3 & 0xF0) >> 4 << 4) | ((byte_2 & 0x0F) >> 0 << 0);
  }

  // i=1 -> IGNORE FIRST 2
  // Read 1536 double entries (sets of 3 bytes)
  // from first fat (3072 total)
  for(int i = 1; i < 1536; i++) {
    byte_1 = second_raw_fat_data[i * 3];
    byte_2 = second_raw_fat_data[(i * 3) + 1];
    byte_3 = second_raw_fat_data[(i * 3) + 2];

    // Process Little Endianness
    // NOTE <<<THIS MIGHT NOT WORK>>>
    fat.entries[3072 + i*2] =     ((byte_2 & 0xF0) >> 4 << 8) | ((byte_1 & 0x0F) >> 0 << 4) | ((byte_1 & 0xF0) >> 4 << 0);
    fat.entries[3072 + i*2 + 1] = ((byte_3 * 0x0F) >> 0 << 8) | ((byte_3 & 0xF0) >> 4 << 4) | ((byte_2 & 0x0F) >> 0 << 0);
  }

  return fat;
}

void print_diskinfo_fat12(disk_fat12 * disk) {
  printf("OS Name: %s \n" "Boot Signature: %d\n" "Volume ID: %s\n"
	 "Volume Label: %s\n" "File System Type: %s\n" "Num Heads: %d\n"
	 "Bytes per Sector: %d\n" "Sectors per Fat: %d\n"
	 "Sectors per Cluster: %d\n" "Sectors per Track: %d\n"
	 "Num Reserved Sectors: %d\n" "Total Sector Count: %d\n"
         "Number of FATs: %d\n" "Max root directory entries: %d\n",
	 disk->diskinfo.os_name, disk->diskinfo.boot_sig,
	 disk->diskinfo.vol_id, disk->diskinfo.vol_label, disk->diskinfo.fs_type,
	 disk->diskinfo.num_heads, disk->diskinfo.bytes_per_sector,
	 disk->diskinfo.sectors_per_fat, disk->diskinfo.sectors_per_cluster,
	 disk->diskinfo.sectors_per_track, disk->diskinfo.num_reserved_sectors,
	 disk->diskinfo.total_sector_count, disk->diskinfo.num_fats,
	 disk->diskinfo.max_rootdir_entries);	 
}

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

  return disk;
}

int diskinfo_freesize_fat12(disk_fat12 * disk) {
  // For each entry in the FAT table.
  // Count all 0x00 entries
  return -1;
}

int diskinfo_totalfilecount_fat12(disk_fat12 * disk) {
  // for each entrie in the root directory
  //  count++
  //  if directory, recursively count files in directories
  //    (excluding '.' and '..')
  return -1;
}

int disk_is_directory(du_file * file) {
  return (file->attr & 0x10);
}

void disk_print_file(du_file * file, FILE * stream) {
  return;
}

// Individual file retrieval.
du_file *  disk_file_from_path(char * path, disk_fat12 * disk) {
  return NULL;
}

// Getting Directory Listings.
du_file ** disk_list_from_path(char * path, disk_fat12 * disk) {
  return NULL;
}

disk_file_sector_map disk_get_file_sector_map(du_file * file, disk_fat12 * disk) {
  disk_file_sector_map dfsm;

  dfsm.map = (int16_t *) malloc(sizeof(int16_t) * 64);

  int index = 0;
  // last <- first FAT entry position from file
  int16_t fat_index = file->first_logical_cluster;
  // append last to dfsm
  dfsm.map[index++] = disk->fat.entries[fat_index];

  // while FAT[ value at last fat position] is valid, 0x000 < x < 0xFF0
  //  last <- FAT[ value at last fat position]
  //  append last to dfsm

  return dfsm;
}

char * disk_get_file_contents(du_file * file) {
  return NULL;
}

du_file ** disk_list_from_directory(du_file * file, disk_fat12 * disk) {
  return NULL;
}

// Individual file insertion.
int disk_file_to_path(char * path, disk_fat12 * disk) {
  return -1;
}
int disk_file_to_directory(du_file * file, disk_fat12 * disk) {
  return -1;
}

disk_file_sector_map disk_file_sector_map_init() {
  disk_file_sector_map dfsm;
  dfsm.map = (int16_t *) malloc(sizeof(int16_t) * 64);
  if(dfsm.map == NULL) {
    fprintf(stderr, "Error: malloc\n");
    exit(EXIT_FAILURE);
  }
  dfsm.map_max_size = 64;
  dfsm.map_size = 0;
  return dfsm;
}

void add_to_disk_file_sector_map(disk_file_sector_map * dfsm, int16_t sector) {
  if(dfsm->map_max_size == dfsm->map_size) {
    dfsm->map_max_size *= 2;
    dfsm->map = (int16_t *) realloc(dfsm->map, sizeof(int16_t) * dfsm->map_max_size);
    if(dfsm->map == NULL) {
      fprintf(stderr, "Error: malloc\n");
      exit(EXIT_FAILURE);
    }
  }
  dfsm->map[dfsm->map_size++] = sector;
}