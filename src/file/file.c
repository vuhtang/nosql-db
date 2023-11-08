#include "file/file.h"

status file_add_entity(struct Object *obj, struct File *file, file_off *file_addr) {
    // init entity from object
    // write entity to the file
    // write other entities from the given object like sibling or son
    return OK;
}

struct File_header* init_header_struct() {
    return malloc(sizeof(struct File_header));
}

struct File* init_file_struct() {
    return malloc(sizeof(struct File));
}

struct File* init_file(FILE *fp) {
    struct File_header *fh = init_header_struct(fp);
    fh->fileptr = fp;
    fh->file_size = 0;
    struct File *f = init_file_struct();
    f->header = fh;
    return f;
}