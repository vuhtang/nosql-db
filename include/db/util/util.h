
#ifndef LLP_UTIL_H
#define LLP_UTIL_H

#include "db/types/types.h"
#include "stdlib.h"
#include "string.h"
#include <stdbool.h>
#include <stdio.h>

enum status {
    OK = 0,
    ERROR = 1
};

bool strings_are_equals(struct string *s1, struct string *s2);
struct string *init_string(char *c , size_t len);
struct string *string_concat_with_dot(struct string *s1, struct string *s2);
void free_string(struct string *str);
void print_status(enum status s);
bool check_values_with_operation(union raw_value v1, union raw_value v2, enum object_type type, enum query_value_cmp_op op);

#endif