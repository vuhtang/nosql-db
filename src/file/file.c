#include "file/file.h"
#include "section/section_header.h"

struct file_header *init_header_struct() {
    return malloc(sizeof(struct file_header));
}

struct file *init_file_struct() {
    return malloc(sizeof(struct file));
}

struct file *init_file(int fd) {
    struct file_header *fh = init_header_struct();
    fh->file_desc = fd;
    fh->file_size = 0;
    fh->root_object_addr = 0;
    fh->last_root_layer_addr = 0;

    struct file *file = init_file_struct();
    file->header = fh;

    add_service_section(file);
    sync_file_header(file);

    return file;
}

void file_read_all_sections(struct file *file) {
    if (file->header->file_size <= SECTION_SIZE) return;
    file_off cur_section_addr = SECTION_SIZE;

    struct section_region *first_region = malloc(sizeof(struct section_region));
    file->first_region = first_region;
    struct section_header *header = malloc(sizeof(struct section_header));

    section_read_region_header(cur_section_addr, file, header);
    first_region->header = header;

    cur_section_addr += SECTION_SIZE;

    struct section_region *cur_region = NULL;
    struct section_region *prev_region = first_region;

    while (cur_section_addr < file->header->file_size) {
        cur_region = malloc(sizeof(struct section_region));
        header = malloc(sizeof(struct section_header));
        section_read_region_header(cur_section_addr, file, header);
        prev_region->next = cur_region;
        prev_region = cur_region;
        cur_section_addr += SECTION_SIZE;
    }

    file->last_region = prev_region;
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

    file_read_all_sections(file);
    return file;
}

void file_add_section(struct file *file, struct section_region *section) {
    if (!file->first_region) {
        file->first_region = section;
        file->last_region = section;
    } else if (file->first_region == file->last_region) {
        file->first_region->next = section;
        file->last_region = section;
    } else {
        file->last_region->next = section;
        file->last_region = section;
    }
}

void del_header(struct file_header *header) {
    close(header->file_desc);
    free(header);
}

void del_regions(struct file *file) {
    struct section_region *reg = file->first_region;
    while (reg) {
        struct section_region *tmp = reg->next;
        free(reg);
        reg = tmp;
    }
}

void del_file(struct file *fp) {
    del_header(fp->header);
    del_regions(fp);
    free(fp);
}