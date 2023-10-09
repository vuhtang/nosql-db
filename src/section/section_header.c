//
// Created by Иван Варюхин on 07.10.2023.
//

#include "section/section_header.h"

struct section_header {
    size_t free_space;
    struct section_header *next_section;
    void *first_free_cell;
    void *last_free_cell;
};

void* init_section_header(FILE* fp) {
    int fd = fileno(fp);
    void *section_start = mmap(NULL, SECTION_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    return section_start;
}

int delete_section_header(void *sp) {
    return munmap(sp, HEADER_SIZE);
}

