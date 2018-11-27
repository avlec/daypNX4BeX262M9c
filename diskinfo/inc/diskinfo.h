#ifndef DISKINFO_H
#define DISKINFO_H

#include "diskutil.h"

unsigned int count_files(disk_fat12 * disk);

unsigned int count_freesize(disk_fat12 * disk);

#endif
