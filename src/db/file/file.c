#include "db/file/file.h"
#include "db/section/section_header.h"
#define HEAP_CAPACITY 8

struct file_header *init_header_struct() {
    return malloc(sizeof(struct file_header));
}

struct file *init_file_struct() {
    return malloc(sizeof(struct file));
}

struct sections_heap *init_sections_heap() {
    struct sections_heap *heap = malloc(sizeof(struct sections_heap));
    heap->size = 0;
    heap->capacity = HEAP_CAPACITY;
    heap->arr = malloc(sizeof(struct heap_elem) * HEAP_CAPACITY);
    return heap;
}

struct file *init_file(int fd) {
    struct file_header *fh = init_header_struct();
    fh->file_desc = fd;
    fh->file_size = 0;
    fh->root_object_addr = 0;
    fh->last_root_layer_addr = 0;

    struct file *file = init_file_struct();
    file->header = fh;
    file->sections = init_sections_heap();

    add_service_section(file);
    sync_file_header(file);

    return file;
}

void file_read_all_sections(struct file *file) {
    if (file->header->file_size <= SECTION_SIZE) return;
    file_off cur_section_addr = SECTION_SIZE;

    struct section_region *first_region = malloc(sizeof(struct section_region));
    struct section_header *header = malloc(sizeof(struct section_header));
    section_read_region_header(cur_section_addr, file, header);
    first_region->header = header;

    heap_insert(file->sections, heap_elem_from_region(first_region));

    cur_section_addr += SECTION_SIZE;

    struct section_region *cur_region = NULL;

    while (cur_section_addr < file->header->file_size) {
        cur_region = malloc(sizeof(struct section_region));
        header = malloc(sizeof(struct section_header));
        section_read_region_header(cur_section_addr, file, header);
        heap_insert(file->sections, heap_elem_from_region(cur_region));
        cur_section_addr += SECTION_SIZE;
    }
}

struct file *read_file(int fd) {
    struct file_header_entity fhe = section_read_fhe(fd);
    struct file_header *fh = init_header_struct();
    fh->file_desc = fd;
    fh->file_size = fhe.file_size;
    fh->root_object_addr = fhe.root_object_addr;
    fh->last_root_layer_addr = fhe.last_root_layer_addr;

    struct file *file = init_file_struct();
    file->header = fh;
    file->sections = init_sections_heap();

    file_read_all_sections(file);
    return file;
}

void file_add_section(struct file *file, struct section_region *section) {
    heap_insert(file->sections, heap_elem_from_region(section));
}

void sync_section_header_in_file_struct(struct file *file, struct section_header *header) {
    heap_update_elem(file->sections, header);
}

void del_header(struct file_header *header) {
    close(header->file_desc);
    free(header);
}

void del_regions(struct sections_heap *heap) {
    for (size_t i = 0; i < heap->size; ++i) {
        free_heap_elem(&(heap->arr)[i]);
    }
    free(heap->arr);
    free(heap);
}

void del_file(struct file *file) {
    del_header(file->header);
    del_regions(file->sections);
    free(file);
}