#ifndef LLP_TYPES_H
#define LLP_TYPES_H

#include "inttypes.h"
#include "util/util.h"
#include <stdbool.h>

typedef size_t file_off;
typedef size_t section_off;

typedef struct String {
    char *ptr;
    size_t len;
} String;

enum value_type {
    VAL_BOOL = 0,
    VAL_INT_32,
    VAL_FLOAT,
    VAL_STRING
};

union raw_value {
    int32_t int32;
    bool aBool;
    float aFloat;
    String *string;
};

enum object_type {
    OBJ_INT32 = 0,
    OBJ_BOOL,
    OBJ_FLOAT,
    OBJ_STRING,
    OBJ_OBJ
};

#endif//LLP_TYPES_H
