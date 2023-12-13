
#ifndef LLP_UTIL_H
#define LLP_UTIL_H

#include "stdlib.h"
#include "string.h"
#include "types/types.h"
#include <stdbool.h>
#include <stdio.h>

enum status {
    OK = 0,
    ERROR = 1
};

bool strings_are_equals(struct string *s1, struct string *s2);
struct string *init_string(char *c , size_t len);
void free_string(struct string *str);
void print_status(enum status s);

#endif