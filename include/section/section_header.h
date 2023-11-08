//
// Created by Иван Варюхин on 08.10.2023.
//

#ifndef LLP_SECTION_HEADER_H
#define LLP_SECTION_HEADER_H

#include "sys/mman.h"
#include "stdio.h"
#include "types.h"

#define HEADER_SIZE sizeof(struct section_header)
#define SECTION_SIZE 8192

struct section_region {
    struct section_header header;
    struct section_region *next;
};

struct section_header {
    size_t free_space;
    section_off first_free_cell;
    section_off last_free_cell; 
    file_off section_addr; // offset from file start
};

void* init_section_header(FILE* fp);
int delete_section_header(void *sp);

#endif//LLP_SECTION_HEADER_H
