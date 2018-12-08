#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "diskutil.h"
#include "diskget.h"

/* Borrowed code from:
 *	www.programmingsimplified.com/c/program/c-program-change-case
 */
void upper_string(char * str) {
	int c = 0;
	while(str[c] != '\0') {
		if(str[c] >= 'a' && str[c] <= 'z') {
			str[c] = str[c] - 32;
		}
		c++;
	}
}

void convert_token(char * token, char * name, char * ext) {
	upper_string(token);

	int j = 0; // index in token string

	for(int i = 0; i < 8; ++i) {
		if(token[j] != '\0' && token[j] != '.')
			name[i] = token[j++];
		else
			name[i] = ' ';
	}
	name[8] = '\0';

	if(token[j++] == '.') {
		
		for(int i = 0; i < 3; ++i) {
			if(token[j] != '\0')
				ext[i] = token[j++];
			else
				ext[i] = ' ';
		}
  } else {
		for(int i = 0; i < 3; ++i) {
			ext[i] = ' ';
		}
	}
	ext[3] = '\0';
}

const char * delim = "/";

// Individual file retrieval.
du_file r_disk_file_from_path(char * token, FILE * input, disk_fat12 * disk, int16_t sector) {
	char name[9];
	char ext[4];
	convert_token(token, name, ext);

	du_file file = DU_FILE_INITIALIZER;
	fseek(input, 512*sector, SEEK_SET);
	char * entry = (char *) malloc(sizeof(char) * 32);
	fread(entry, sizeof(char), 32, input);

	if(entry[0] == 0xE5) {
		fclose(input);
		return file;
	}

	// DEBUG fprintf(stderr, "looking for [%s.%s]\n", name, ext);

	while(entry[0] != 0x00) {
		file = new_du_file(entry, disk);

		// DEBUG fprintf(stderr, "checking [%s.%s]\n", name, ext);

		if(disk_is_file_valid(&file)) {
			if(strncmp(file.name, name, (strlen(file.name) < 8) ? strlen(file.name) : 8) == 0) {
				if(strncmp(file.ext, ext, (strlen(file.ext) < 3)  ? strlen(file.name) : 3) == 0) {
					// Found first part of string
					token = strtok(NULL, delim);
					if((file.attr & 0x10) != 0) {
						// Directory
						fclose(input);
						return r_disk_file_from_path(token, input, disk, file.first_logical_cluster);
					}
					if(token == NULL) {
						// Found!
						fclose(input);
						return file;
					}
				}
			}
		}
		fread(entry, sizeof(char), 32, input);
	}
	file = DU_FILE_INITIALIZER;
	fclose(input);
	return file;
}

du_file disk_file_from_path(char * path, disk_fat12 * disk) {
	char * token = strtok(path, delim);

	FILE * input = fopen(disk->mount_point, "rb");
	if(input == NULL) {
		fprintf(stderr, "Cannot open file \n");
		perror("Error: ");
		exit(1);
	}
  return r_disk_file_from_path(token, input, disk, 19);
}

void write_file_contents(du_file * file, disk_fat12 * disk, char * file_location) {
	FILE * output = fopen(file_location, "wb+");
	if(output == NULL) {
		fprintf(stderr, "Error opening output file\n");
		exit(1);
	}

	FILE * input = fopen(disk->mount_point, "rb");
	if(input == NULL) {
		fprintf(stderr, "Error opening input file\n");
		exit(1);
	}
	
	char * buffer = (char *) malloc(sizeof(char) * 512);
	if(buffer == NULL) {
		fprintf(stderr, "Error malloc\n");
		exit(1);
	}

	for(int i = 0; i < file->dfsm.map_size; ++i) {
		fseek(input, (31 + file->dfsm.map[i]) * 512, SEEK_SET);
		fread(buffer, sizeof(char), 512, input);
		fwrite(buffer, sizeof(char), 512, output);
	}

	fclose(input);
	fclose(output);
}

int main(int argc, char ** argv) {
	if(argc != 3) {
    fprintf(stderr, "Invoke with: $ diskget diskimage.IMA file/path/to/file.ext");
    exit(EXIT_FAILURE);
  }

	disk_fat12 disk = new_disk_fat12(argv[1]);
	du_file file = disk_file_from_path(argv[2], &disk);

	if(!disk_is_file_valid(&file)) {
		fprintf(stderr, "File not found at path specified.\n");
		return EXIT_FAILURE;
	}
	write_file_contents(&file, &disk, argv[2]);
	return EXIT_SUCCESS;
}
