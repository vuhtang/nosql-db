#ifndef LLP_FILE_H
#define LLP_FILE_H

#include "heap.h"
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
    struct sections_heap *sections;
};

struct file_header_entity {
    size_t file_size;
    file_off root_object_addr;
    file_off last_root_layer_addr;
};

struct file *init_file(int fd);
void file_add_section(struct file *file, struct section_region *section);
struct file *read_file(int fd);
void sync_section_header_in_file_struct(struct file *file, struct section_header *header);
void del_file(struct file *fp);

#endif