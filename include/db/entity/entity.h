#ifndef LLP_ENTITY_H
#define LLP_ENTITY_H

#include "stdlib.h"
#include "db/types/types.h"
#include "db/util/util.h"

struct object {
    file_off addr;
    struct string *key;
    enum object_type type;
    union raw_value raw_value;
    struct object *ancestor;// ptr to ancestor node
    struct object *child;   // ptr to first child
    struct object *sibling; // ptr to next child of ancestor
};

struct entity {
    section_off key_ptr;
    size_t key_size;
    section_off val_ptr;
    size_t val_size;
    enum value_type val_type;
    file_off ancestor_ptr;
    file_off child_ptr;
    file_off sibling_ptr;
};

struct entity entity_from_object(struct object *object, file_off ancestor_ptr);
struct object *object_from_entity(struct entity entity, file_off entity_addr, struct string *key, union raw_value value);

#endif