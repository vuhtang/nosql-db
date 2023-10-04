//
// Created by Иван Варюхин on 26.09.2023.
//

#ifndef LLP_TYPES_H
#define LLP_TYPES_H

#include "inttypes.h"
#include <stdbool.h>

enum value_type {
    BOOL = 0,
    INT_32,
    FLOAT,
    STRING
};

union raw_value {
    int32_t int32;
    bool aBool;
    float aFloat;
    char* string;
};

struct node_value {
    enum value_type type;
    union raw_value value;
};

struct node {
    
};

#endif//LLP_TYPES_H
