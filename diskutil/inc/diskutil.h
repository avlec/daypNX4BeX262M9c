#ifndef DISKUTIL_H
#define DISKUTIL_H

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
 * This structure defines the logical representation of a disk.
 * 
 * @param diskinfo; information about the disk
 * @param mount_point; the absolute path to the location of the disk
 *
 * @TODO Is there anything else really required for the disk information.
 */
typedef struct disk_fat12 {
  struct diskinfo_fat12 diskinfo;
  char * mount_point;
} disk_fat12;

/**
 * This strucutre defines the logical representation of a
 * directories contents.
 * 
 * @param files: list of file paths in the directories.
 * @param count: a count of the number of file paths.
 */
typedef struct du_dir_content {
  char ** files;
  unsigned int count;
} du_dir_content;

/**
 * This structure defines the logical representation of
 * a file or directory.
 * 
 * @param name; name of the file/directory.
 * @param path; place on the disk.
 * @param type; either file 'F' or directory 'D'.
 * @param dir_content; list of files if this is a 
 *    directory (type == 'D'). Otherwise this is all zeroes.
 * @param bytes; size of file/directory in bytes.
 * @param creation_date; date file/directory was created.
 * @param creation_time; time file/directory was created.
 */
typedef struct du_file {
  char * name;
  char * path;
  char type;
  struct du_dir_content dir_content; // Unused if type != 'D'
  unsigned int bytes;
  char * creation_date;
  char * creation_time;
} du_file;

#define DU_FILE_INITIALIZER { .name = NULL, .path = NULL, .type = 'X', .dir_content = { .files = NULL, .count = 0 }, .bytes = 0, .creation_date = NULL, .creation_time = NULL }

typedef struct QNode {
	struct QNode * next;
	struct QNode * prev;
	struct du_file * file;
} QNode;

typedef struct Queue {
	struct QNode * head;
	struct QNode * tail;
	unsigned int length;
} Queue;

#define QUEUE_INITIALIZER (Queue) { .head = NULL, .tail = NULL, .length = 0 }

int q_push(Queue * queue, du_file * file);
du_file * p_pop(Queue * queue);
du_file * q_peek(Queue * queue);

// Function definitions.
diskinfo_fat12 get_diskinfo_fat12(char * bootsector_raw_data);
void print_diskinfo_fat12(disk_fat12 * disk);
disk_fat12 new_disk_fat12(char * file_location);
int diskinfo_freesize_fat12(disk_fat12 * disk);
int diskinfo_totalfilecount_fat12(disk_fat12 * disk);
int diskinfo_numfatcopies_fat12(disk_fat12 * disk);

#endif
