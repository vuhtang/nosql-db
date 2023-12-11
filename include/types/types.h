#ifndef LLP_TYPES_H
#define LLP_TYPES_H

#include "inttypes.h"
#include <stdbool.h>
#include <stddef.h>

typedef size_t file_off;
typedef size_t section_off;

struct string {
    char *ptr;
    size_t len;
};

union raw_value {
    int32_t int32;
    bool aBool;
    float aFloat;
    struct string string;
};

enum query_value_type {
    Q_VAL_BOOL = 0,
    Q_VAL_INT32,
    Q_VAL_FLOAT,
    Q_VAL_STRING
};

enum query_value_cmp_op {
    EQUALS,
    SET_VAL
};

enum value_type {
    VAL_BOOL = 0,
    VAL_INT32,
    VAL_FLOAT,
    VAL_STRING,
    VAL_OBJ
};

enum object_type {
    OBJ_INT32 = 0,
    OBJ_BOOL,
    OBJ_FLOAT,
    OBJ_STRING,
    OBJ_OBJ
};

#endif//LLP_TYPES_H
