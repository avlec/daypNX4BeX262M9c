#include <stdlib.h>
#include <stdio.h>

#include "disklist.h"
#include "diskutil.h"
#include "filequeue.h"

void list_allfiles(disk_fat12 * disk) {
  // list dir/files on root directory
  du_file * pwd = disk_file_from_path("\\", disk);
  // pwd execute recursive printing on pwd
  _r_list_all_files(pwd, disk);
  free(pwd); // Assuming disk_get allocates new memory
}

void _r_list_all_files(du_file * pwd, disk_fat12 * disk) {
  if(!disk_is_directory(pwd)) {
    return;
  }
  // Print Directory Label, and seperator.
  printf("%s\n"
	 	"\"==================\"\n",
		pwd->name);
	
  // Get list of files to print.
  du_file ** result = disk_list_from_directory(pwd, disk);

  // for each file in result, print
  for(int i = 0; result[i] != NULL; ++i) {
    disk_print_file(pwd, stdout);
  }

  // for each file in result, recursively branch
  for(int i = 0; result[i] != NULL; ++i) {
    _r_list_all_files(result[i], disk);
  }
}

int main(int argc, char ** argv) {
	return EXIT_SUCCESS;
}
