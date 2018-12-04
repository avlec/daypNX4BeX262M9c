#include "diskutil.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Debugging
void print_du_file(du_file * file) {
  fprintf(stderr,
  "Name: %s\n"
  "Ext: %s\n"
  "Attr: %d\n"
  "Type: %c\n"
  "Reserved: %d\n"
  "Create Time: %d\n"
  "Create Date: %d\n"
  "Last Access: %d\n"
  "Ignore: %d\n"
  "Write Time: %d\n"
  "Write Date: %d\n"
  "First Logical: %d\n"
  "File Size: %d\n",
  file->name, file->ext, file->attr, ((file->attr&0x10) != 0 ? 'D' : 'F'),
  file->reserved, file->creation_time, file->creation_date, file->last_access_date,
  file->ignore, file->last_write_time, file->last_write_date, file->first_logical_cluster,
  file->file_size);

  fprintf(stderr, "FAT Glue (%d):\n", file->dfsm.map_size);
  for(int i = 0; i < file->dfsm.map_size; ++i) {
    fprintf(stderr, "%d->", file->dfsm.map[i]);
  }
  fprintf(stderr,"\n=========\n");
}

int disk_is_file_valid(du_file * file) {
  if(file->file_size < 0)
    return 0; // Invalid FileSize
  if(file->first_logical_cluster > 3072 || file->first_logical_cluster < 0)
    return 0; // Invalid Cluster
  if(file->file_size > file->dfsm.map_size*512)
    return 0; // Uses more space than it has.
  return 1;
}

void disk_print_file(du_file * file) {
  printf("%c %010d %s.%s %d %d\n",
    ((file->attr & 0x10) != 0) ? 'D' : 'F', file->file_size,
    file->name, file->ext, file->creation_time, file->creation_date);
}

du_file new_du_file(char * raw_file_content, disk_fat12 * disk) {
  du_file file = DU_FILE_INITIALIZER;
  memcpy(file.name, raw_file_content, 8);
  file.name[8] = '\0';
  memcpy(file.ext, raw_file_content+8, 3);
  file.ext[3] = '\0';
  memcpy(&file.attr, raw_file_content+11, 1);
  file.attr = le16toh(file.attr);
  memcpy(&file.reserved, raw_file_content+12, 2);
  file.reserved = le16toh(file.reserved);
  memcpy(&file.creation_time, raw_file_content+14, 2);
  file.creation_time = le16toh(file.creation_time);
  memcpy(&file.creation_date, raw_file_content+16, 2);
  file.creation_date = le16toh(file.creation_date);
  memcpy(&file.last_access_date, raw_file_content+18, 2);
  file.last_access_date = le16toh(file.last_access_date);
  memcpy(&file.ignore, raw_file_content+20, 2);
  file.ignore = le16toh(file.ignore);
  memcpy(&file.last_write_time, raw_file_content+22, 2);
  file.last_write_time = le16toh(file.last_write_time);
  memcpy(&file.last_write_date, raw_file_content+24, 2);
  file.last_write_date = le16toh(file.last_write_date);
  memcpy(&file.first_logical_cluster, raw_file_content+26, 2);
  file.first_logical_cluster = le16toh(file.first_logical_cluster);
  memcpy(&file.file_size, raw_file_content+28, 4);
  file.file_size = le32toh(file.file_size);

  file.dfsm = disk_file_sector_map_init();
  fill_disk_file_sector_map(&file.dfsm, file.first_logical_cluster, disk);

  //TODO: Error checking before returning
  if(!disk_is_file_valid(&file)) {
    file = DU_FILE_INITIALIZER;
  }

  return file;
}

