#ifndef LLP_FILE_H
#define LLP_FILE_H

#include "section/section_header.h"
#include "types/types.h"
#include <stdlib.h>
#include <unistd.h>

struct section_region;

struct file_header {
    int file_desc;
    size_t file_size;
    file_off root_object_addr;    // the first object without ancestor
    file_off last_root_layer_addr;// the last object without ancestor
};

struct file {
    struct file_header *header;
    struct section_region *first_region;
    struct section_region *last_region;
};

struct file_header_entity {
    size_t file_size;
    file_off root_object_addr;
    file_off last_root_layer_addr;
    //TODO heap with free space in sections
};

struct file *init_file(int fd);
void file_add_section(struct file *file, struct section_region *section);
struct file *read_file(int fd);
void del_file(struct file *fp);

#endif