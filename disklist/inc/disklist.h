#ifndef DISKLIST_H
#define DISKLIST_H

#include "diskutil.h"

void list_allfiles(disk_fat12 * disk);
void print_file(du_file * file);
void _r_list_all_files(du_file * pwd, disk_fat12 * disk);

#endif //DISKLIST_H