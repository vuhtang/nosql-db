#ifndef LLP_FILE_H
#define LLP_FILE_H

#include "types/types.h"
#include <stdlib.h>

struct File_header {
    FILE *fileptr;
    size_t file_size;
};

struct File {
    struct File_header header;
    struct section_region *first_region;
};

status file_add_entity(struct Object *obj, struct File *file, file_off *file_addr);
struct File* init_file(FILE *fp);

#endif