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

void print_diskinfo_fat12(diskinfo_fat12 diskinfo) {
  printf("OS Name: %s \n"
	 "Boot Signature: %d\n"
	 "Volume ID: %s\n"
	 "Volume Label: %s\n"
	 "File System Type: %s\n"
	 "Num Heads: %d\n"
         "Bytes per Sector: %d\n"
         "Sectors per Fat: %d\n"
         "Sectors per Cluster: %d\n"
         "Sectors per Track: %d\n"
         "Num Reserved Sectors: %d\n"
         "Total Sector Count: %d\n"
         "Number of FATs: %d\n"
         "Max root directory entries: %d\n",
	 diskinfo.os_name, diskinfo.boot_sig,
	 diskinfo.vol_id, diskinfo.vol_label, diskinfo.fs_type,
	 diskinfo.num_heads, diskinfo.bytes_per_sector,
	 diskinfo.sectors_per_fat, diskinfo.sectors_per_cluster,
	 diskinfo.sectors_per_track, diskinfo.num_reserved_sectors,
	 diskinfo.total_sector_count, diskinfo.num_fats,
	 diskinfo.max_rootdir_entries);	 
}

void print_me() {
  FILE * file = fopen("disk3.IMA", "rb");
  if(file == NULL) {
    printf("Error opening file, exiting.\n");
    exit(1);
  }
  
  char bootsector_raw_data[62];
  fread(bootsector_raw_data, sizeof(char), 62, file);

  diskinfo_fat12 diskinfo = get_diskinfo_fat12(bootsector_raw_data);

  print_diskinfo_fat12(diskinfo);
}
