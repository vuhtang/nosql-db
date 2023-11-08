#include "user.h"
#include "util.h"
#include "file.h"

status user_add_entity(struct File *file, struct Object *obj) {
    file_off file_addr;
    return file_add_entity(file, obj, &file_addr);
}
