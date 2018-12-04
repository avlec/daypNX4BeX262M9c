#include "diskutil.h"

#include <stdlib.h>

fat_fat12 get_fat_fat12(char * first_raw_fat_data, char * second_raw_fat_data) {
  fat_fat12 fat;
  
  // read 24 bits at a time
  int8_t byte_1 = 0;
  int8_t byte_2 = 0;
  int8_t byte_3 = 0;


  // i=1 -> IGNORE FIRST 2
  // Read 1536 double entries (sets of 3 bytes)
  // from first fat (3072 total)
  for(int i = 1; i < 1536; i++) {
    byte_1 = first_raw_fat_data[i * 3];
    byte_2 = first_raw_fat_data[(i * 3) + 1];
    byte_3 = first_raw_fat_data[(i * 3) + 2];

    // Process Little Endianness
    // NOTE <<<THIS MIGHT NOT WORK>>>
    fat.entries[i*2] =     (byte_2 & 0x0F) << 8 | byte_1;
    fat.entries[i*2 + 1] = (byte_3 << 4) | ((byte_2 & 0xF0) >> 4);
  }

  // i=1 -> IGNORE FIRST 2
  // Read 1536 double entries (sets of 3 bytes)
  // from first fat (3072 total)
  for(int i = 1; i < 1536; i++) {
    byte_1 = second_raw_fat_data[i * 3];
    byte_2 = second_raw_fat_data[(i * 3) + 1];
    byte_3 = second_raw_fat_data[(i * 3) + 2];

    // Process Little Endianness
    // NOTE <<<THIS MIGHT NOT WORK>>>
    fat.entries[3072 + i*2] =     (byte_2 & 0x0F) << 8 | byte_1;
    fat.entries[3072 + i*2 + 1] = (byte_3 << 4) | ((byte_2 & 0xF0) >> 4);
  }

  return fat;
}

