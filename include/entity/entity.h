#ifndef LLP_ENTITY_H
#define LLP_ENTITY_H

#include "types.h"

struct Object {
    file_off addr;
    struct String *key;
    struct object_type type;
    struct raw_value raw_value;
    struct Object *ancestor; // ptr to ancestor node
    struct Object *child; // ptr to first child
    struct Oblect *sibling; // ptr to next child of ancestor
    //struct Object *next;
};

struct Entity {
    section_off key_ptr;
    size_t key_value;
    section_off val_ptr;
    size_t val_size;
    struct value_type val_type;
    file_off ancestor_ptr;
    file_off child_ptr;
    file_off sibling_ptr;
};

#endif