#include "section/section_header.h"

#define HEADER_SIZE sizeof(struct section_header)
#define SERVICE_SECTION_ADDR 0

struct section_header *section_init_header() {
    struct section_header *header = malloc(HEADER_SIZE);
    header->free_space = SECTION_SIZE - HEADER_SIZE;
    header->first_free_cell = HEADER_SIZE;
    header->last_free_cell = SECTION_SIZE;
    return header;
}

struct section_region *section_init_region() {
    return malloc(sizeof(struct section_region));
}

struct section_region *section_init() {
    struct section_region *section = section_init_region();
    section->header = section_init_header();
    section->next = NULL;
    return section;
}

struct file_header_entity section_read_fhe(int fd) {
    struct file_header_entity fhe = {};
    void *p = mmap(NULL, SECTION_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, SERVICE_SECTION_ADDR);

    memcpy(&fhe, p, sizeof(fhe));

    msync(p, SECTION_SIZE, MS_SYNC);
    munmap(p, SECTION_SIZE);

    return fhe;
}

void section_read_region_header(file_off section_addr, struct file *file, struct section_header *header) {
    void *p = mmap(NULL, SECTION_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, file->header->file_desc, section_addr);

    memcpy(header, p, sizeof(struct section_header));

    msync(p, SECTION_SIZE, MS_SYNC);
    munmap(p, SECTION_SIZE);
}

struct file_header_entity header_entity_from_file(struct file *file) {
    struct file_header_entity entity = {};
    entity.file_size = file->header->file_size;
    entity.root_object_addr = file->header->root_object_addr;
    entity.last_root_layer_addr = file->header->last_root_layer_addr;
    return entity;
}

void sync_file_header(struct file *file) {
    struct file_header_entity header_entity = header_entity_from_file(file);

    int fd = file->header->file_desc;
    void *p = mmap(NULL, SECTION_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, SERVICE_SECTION_ADDR);
    memcpy(p, &header_entity, sizeof(struct file_header_entity));
    msync(p, SECTION_SIZE, MS_SYNC);
    munmap(p, SECTION_SIZE);
}

void add_service_section(struct file *file) {
    ftruncate(file->header->file_desc, SECTION_SIZE);
    file->header->file_size += SECTION_SIZE;
    sync_file_header(file);
}

void add_new_section(struct file *file) {
    struct section_region *section = section_init();
    int fd = file->header->file_desc;

    section->header->section_addr = file->header->file_size;

    ftruncate(fd, file->header->file_size + SECTION_SIZE);
    void *p = mmap(NULL, SECTION_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, file->header->file_size);
    memcpy(p, section->header, sizeof(struct section_header));
    msync(p, SECTION_SIZE, MS_SYNC);
    munmap(p, SECTION_SIZE);

    file->header->file_size += SECTION_SIZE;

    file_add_section(file, section);
}

void sync_section_header_in_file_struct(struct file *file, struct section_header *header) {
    struct section_region *cur_region = file->first_region;
    while (cur_region->header->section_addr != header->section_addr)
        cur_region = cur_region->next;

    if (cur_region) {
        cur_region->header->free_space = header->free_space;
        cur_region->header->first_free_cell = header->first_free_cell;
        cur_region->header->last_free_cell = header->last_free_cell;
    }
}

