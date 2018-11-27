#include <stdlib.h>
#include <stdio.h>

#include "disklist.h"
#include "diskutil.h"

void print_file(du_file * file) {
  printf("%c %10d %.20s %s %s\n", file->type,
	 file->bytes, file->name, file->creation_date,
	 file->creation_time);
}

void list_allfiles(disk_fat12 * disk) {
  // list dir/files on root directory
  du_file * pwd = disk_get("\\", disk);
  // pwd execute recursive printing on pwd
  _r_list_all_files(pwd, disk);
  free(pwd); // Assuming disk_get allocates new memory
}

void _r_list_all_files(du_file * pwd, disk_fat12 * disk) {
  if(pwd->type == 'F') {
    return;
  }
  // Print Directory Label, and seperator.
  printf("%s\n"
	 	"\"==================\"\n",
		pwd->name);
	
  // Get list of files to print.
  Queue queue = QUEUE_INITIALIZER;
  for(int i = 0; i < pwd->dir_content.count; ++i) {
    du_file * curr = disk_get();
    q_push(&queue, curr);
    print_file(curr);
    _r_list_all_files(curr, disk);
  }
}

int main(int argc, char ** argv) {
	return EXIT_SUCCESS;
}
