//
// Created by Иван Варюхин on 08.10.2023.
//

#ifndef LLP_SECTION_HEADER_H
#define LLP_SECTION_HEADER_H

#include "sys/mman.h"
#include "stdio.h"

#define HEADER_SIZE sizeof(struct section_header)
#define HEAP_START ((void*)0x04040000)
#define SECTION_SIZE 8192

void* init_section_header(FILE* fp);
int delete_section_header(void *sp);

#endif//LLP_SECTION_HEADER_H
