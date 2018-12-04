#ifndef DISKUTIL_H
#define DISKUTIL_H

#include <stdio.h>
#include <stdint.h>

/** 
 * This structure allows for a direct read raw data
 * from the disk into memory
 * then copy over to an actual storage in diskinfo_fat12
 */
typedef struct bootsector_raw_fat12 {
  int8_t endianness[3];
  int8_t os_name[8];
  int16_t bytes_per_sector;
  int8_t sectors_per_cluster;
  int16_t num_reserved_sectors;
  int8_t num_fats;
  int16_t max_rootdir_entries;
  int16_t total_sector_count;
  int8_t ignore_2;
  int16_t sectors_per_fat;
  int16_t sectors_per_track;
  int16_t num_heads;
  int8_t ignore_3[4];
  int8_t ignore_4[4]; // for FAT32
  int8_t ignore_5[2];
  int8_t boot_sig; // 0x29 indicates the following 3 are present
  int8_t vol_id[4];	// volume serial number
  int8_t vol_label[11]; // matches 11-byte volume label in root dir, no volume label "NO NAME    "
  int8_t fs_type[8]; // "FAT12   ", "FAT16   ", "FAT     "
  // Rest is padding for sector
} bootsector_raw_fat12;

/**
 * This strucutre contains all of the information that was found * inside the bootsector of the disk.
 * 
 * TODO Add conveniency metrics about the disk.
 */
typedef struct diskinfo_fat12 {
  char endianness;
  char os_name[9];
  int8_t boot_sig; // if 0x29 then the rest 3 are valid
  char vol_id[5];
  char vol_label[12];
  char fs_type[9];

  int16_t bytes_per_sector;
  int8_t  sectors_per_cluster;
  int16_t num_reserved_sectors;
  int8_t  num_fats;
  int16_t max_rootdir_entries;
  int16_t total_sector_count;
  int16_t sectors_per_fat;
  int16_t sectors_per_track;
  int16_t num_heads;
} diskinfo_fat12;

/**
 *  This structure holds information found on the disks FAT sectors.
 * 
 *  @param _primary_reserved: holds primary fat's reserved entries
 *  @param _secondary_reserved: holds secondary fat's reserved entries
 *  @param entries: this holds all of the entries in the fat table.
 */
typedef struct fat_fat12 {
  int16_t _primary_reserved_1;
  int16_t _primary_reserved_2;
  int16_t _secondary_reserved_1;
  int16_t _secondary_reserved_2;
  int16_t entries[6144]; // TODO allow for undefined number of fat entries
} fat_fat12;


/**
 *  This structure holds a map of the sectors that contain the contents
 *  of a file/directory.
 * 
 *  @param map: list of sectors of the file in order
 *  @param man_size: number of sectors the file consists of.
 */
typedef struct disk_file_sector_map {
  int16_t * map;
  int16_t map_max_size;
  int16_t map_size;
} disk_file_sector_map;

#define DISK_FILE_SECTOR_MAP_INIT { .map = (int16_t) malloc(sizeof(int16_t) * 64), .map_max_size = 64, .map_size = 0 }

/**
 * This structure defines the logical representation of a disk.
 * 
 * @param diskinfo; information about the disk
 * @param mount_point; the absolute path to the location of the disk
 */
typedef struct disk_fat12 {
  struct diskinfo_fat12 diskinfo;
  struct fat_fat12 fat;
  char * mount_point;
} disk_fat12;

/**
 * This structure defines the logical representation of
 * a file or directory.
 * 
 * @param name
 * @param ext
 * @param attr
 * @param reserved
 * @param creation_tile
 * @param creation date
 * @param last_access_date
 * @param last_write_time
 * @param last_write_date
 * @param first_logical_cluster
 * @param file_size
 */
typedef struct du_file {
  char name[9];
  char ext[4];
  int8_t attr;
  int16_t reserved;
  int16_t creation_time;
  int16_t creation_date;
  int16_t last_access_date;
  int16_t ignore;
  int16_t last_write_time;
  int16_t last_write_date;
  int16_t first_logical_cluster;
  int32_t file_size;

  struct disk_file_sector_map dfsm;
} du_file;

#define DU_FILE_INITIALIZER (du_file)\
  { .name = "FULLINIT", .ext = "NaN", .attr = 0, .reserved = 0, \
  .creation_time = 0, .creation_date = 0, .last_access_date = 0, \
  .ignore = 0, .last_write_time = 0, .last_write_date = 0, \
  .first_logical_cluster = -1, .file_size = 0 }

// Function definitions.

/**
 * Used to create the disk object for the programmer to use.
 */
disk_fat12 new_disk_fat12(char * file_location);

/**
 * This function reads the information from the given
 * copy of the boot sector.
 */
diskinfo_fat12 get_diskinfo_fat12(char * bootsector_raw_data);

/**
 * This function is used to read information from the given
 * copy of the FAT sectors.
 */
fat_fat12 get_fat_fat12(char * first_raw_fat_data, char * second_raw_fat_data);

/**
 * 
 */
du_file new_du_file(char * raw_file_content, disk_fat12 * disk);

/**
 * Prints disk information.
 */
void print_diskinfo_fat12(disk_fat12 * disk);

/**
 * Used to calc disk metrics.
 */
int diskinfo_freesize_fat12(disk_fat12 * disk);
int diskinfo_totalfilecount_fat12(disk_fat12 * disk);

int disk_is_directory(du_file * file);

/**
 * Prints file information.
 */
void disk_print_file(du_file * file);

int disk_is_file_valid(du_file * file);

/**
 * I/O Operations.
 */

// Individual file retrieval.
du_file *  disk_file_from_path(char * path, disk_fat12 * disk);

// Getting Directory Listings.
du_file ** disk_list_from_path(char * path, disk_fat12 * disk);
du_file ** disk_list_from_directory(du_file * file, disk_fat12 * disk);

// Individual file insertion.
int disk_file_to_path(char * path, disk_fat12 * disk);
int disk_file_to_directory(du_file * file, disk_fat12 * disk);

// DFSM functions
disk_file_sector_map disk_file_sector_map_init();
int fill_disk_file_sector_map(disk_file_sector_map * dfsm, int16_t first_logical_cluster, disk_fat12 * disk);
void add_to_disk_file_sector_map(disk_file_sector_map * dfsm, int16_t sector);

#endif
