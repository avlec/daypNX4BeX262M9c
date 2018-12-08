#include "diskutil.h"

#include <string.h>
#include <stdlib.h>

int disk_is_directory(du_file * file) {
  return (file->attr & 0x10);
}


char * disk_get_file_contents(du_file * file) {
  return NULL;
}

// Getting Directory Listings.
du_file ** disk_list_from_path(char * path, disk_fat12 * disk) {
  const char * delim = "/";
  char * token = strtok(path, delim);

  du_file ** ret = (du_file **) malloc(sizeof(du_file *) * 2048);
  if(ret == NULL) {
		fprintf(stderr, "Error allocating memory, exiting.\n");
		exit(1);
	}

  while(token != NULL) {
    
  }

  return ret;
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