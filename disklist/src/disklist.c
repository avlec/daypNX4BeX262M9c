#include <stdlib.h>
#include <stdio.h>

#include "disklist.h"
void list_allfiles(disk_fat12 * disk);
void print_file(du_file * file);
void _r_list_all_files(file * pwd, disk_fat12 * disk);


void print_file(du_file * file) {
  printf("%c %10d %20s %s %s\n", file.type,
	 file.bytes, file.name, file.creation_date,
	 file.creation_time);
}

void list_allfiles(disk_fat12 * disk) {

  // list dir/files on root directory
  du_file * pwd = disk_get("\\", disk);
  print_file(pwd);
  // pwd execute recursive printing on pwd
  _r_list_all_files(pwd, disk);
  free(pwd); // Assuming disk_get allocates new memory
}

void _r_list_all_files(file * pwd, disk_fat12 * disk) {
  printf("%s\n"
	 "\"==================\"\n"
	 
  for(int i = 0; i < pwd->dir_content.count; ++i) {
    du_file * curr = disk_get(pwd->dir_conent.files[i]);
    print_file(curr);
    _r_list_all_files(curr, disk);
  }
}

int main(int argc, char ** argv) {
	return EXIT_SUCCESS;
}
