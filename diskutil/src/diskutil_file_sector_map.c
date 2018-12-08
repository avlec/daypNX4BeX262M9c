#include "diskutil.h"

#include <stdlib.h>
#include <stdio.h>

/**
 *  These functions construct resources to use to access file data.
 */
disk_file_sector_map disk_get_file_sector_map(du_file * file, disk_fat12 * disk) {
  disk_file_sector_map dfsm;

  dfsm.map = (int16_t *) malloc(sizeof(int16_t) * 64);
  if(dfsm.map == NULL) {
		fprintf(stderr, "Error allocating memory, exiting.\n");
		exit(1);
	}

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

disk_file_sector_map disk_file_sector_map_init() {
  disk_file_sector_map dfsm;
  dfsm.map = (int16_t *) malloc(sizeof(int16_t) * 64);
  if(dfsm.map == NULL) {
    fprintf(stderr, "Error allocating memory, exiting.\n");
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
      fprintf(stderr, "Error allocating memory, exiting.\n");
      exit(1);
    }
  }
  dfsm->map[dfsm->map_size++] = sector;
}

int fill_disk_file_sector_map(disk_file_sector_map * dfsm, int16_t first_logical_cluster, disk_fat12 * disk) {
  int16_t curr = first_logical_cluster;
  add_to_disk_file_sector_map(dfsm, curr);
  int16_t next = disk->fat.entries[curr];
  while(0 < next && next < 0xFF0) {
    add_to_disk_file_sector_map(dfsm, next);
    curr = next;
    next = disk->fat.entries[curr];
  }
  if(next > 0xFF0) {
    printf("Error on disk.\n");
    return -1;
  }
  return 0;
}