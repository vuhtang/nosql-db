#ifndef LLP_SECTION_HEADER_H
#define LLP_SECTION_HEADER_H

#include "db/file/file.h"
#include "memory.h"
#include "stdio.h"
#include "sys/mman.h"
#include "sys/types.h"
#include "db/types/types.h"

#define SECTION_SIZE 16384

struct file;

struct section_header {
    size_t free_space;
    section_off first_free_cell;
    section_off last_free_cell; 
    file_off section_addr; // offset from file start
};

struct section_service_header {
    size_t free_space;
};

struct section_region {
    struct section_header *header;
    struct section_region *next;
};

struct section_region *section_init();
void free_section_region(struct section_region *region);
void add_service_section(struct file *file);
void add_new_section(struct file *file);
void sync_file_header(struct file *file);
struct file_header_entity section_read_fhe(int fd);
void section_read_region_header(file_off section_addr, struct file *file, struct section_header *header);

#endif
