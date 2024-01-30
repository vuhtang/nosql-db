#include "db/util/util.h"

bool strings_are_equals(struct string *s1, struct string *s2) {
    if (s1->len != s2->len)
        return false;
    return strncmp(s1->ptr, s2->ptr, s1->len) == 0;
}

struct string *init_string(char *c, size_t len) {
    if (len == 0)
        len = strlen(c);
    char *ptr = malloc(sizeof(char) * len);
    struct string *s = malloc(sizeof(struct string));
    s->ptr = ptr;
    s->len = len;
    //    if (len == 0)
    strcpy(s->ptr, c);
    return s;
}

struct string *string_concat_with_dot(struct string *s1, struct string *s2) {
    size_t len = s1->len + s2->len + 1;
    char *ptr = malloc(sizeof(char) * len);
    strcpy(ptr, s1->ptr);
    strcat(ptr, ".");
    strcat(ptr, s2->ptr);
    struct string *out = malloc(sizeof(struct string));
    out->ptr = ptr;
    out->len = len;
    return out;
}

void free_string(struct string *str) {
    if (str) {
        free(str->ptr);
        free(str);
    }
}

void print_status(enum status s) {
    s == OK ? printf("OK\n") : printf("ERROR\n");
}

bool check_bools_with_operation(bool b1, bool b2, enum query_value_cmp_op op) {
    switch (op) {
        case EQUALS:
            return b1 == b2;
        case LESS:
            return b1 < b2;
        case BIGGER:
            return b1 > b2;
        default:
            return false;
    }
}

bool check_ints_with_operation(int32_t i1, int32_t i2, enum query_value_cmp_op op) {
    switch (op) {
        case EQUALS:
            return i1 == i2;
        case LESS:
            return i1 < i2;
        case BIGGER:
            return i1 > i2;
        default:
            return false;
    }
}

bool check_floats_with_operation(float f1, float f2, enum query_value_cmp_op op) {
    switch (op) {
        case EQUALS:
            return f1 == f2;
        case LESS:
            return f1 < f2;
        case BIGGER:
            return f1 > f2;
        default:
            return false;
    }
}

bool check_strings_with_operation(struct string s1, struct string s2, enum query_value_cmp_op op) {
    switch (op) {
        case EQUALS:
            return strings_are_equals(&s1, &s2);
        case LESS:
            return (strcmp(s1.ptr, s2.ptr) < 0 ? true : false);
        case BIGGER:
            return (strcmp(s1.ptr, s2.ptr) > 0 ? true : false);
        default:
            return false;
    }
}

bool check_values_with_operation(union raw_value v1, union raw_value v2, enum object_type type, enum query_value_cmp_op op) {
    switch (type) {
        case OBJ_BOOL:
            return check_bools_with_operation(v1.aBool, v2.aBool, op);
        case OBJ_INT32:
            return check_ints_with_operation(v1.int32, v2.int32, op);
        case OBJ_FLOAT:
            return check_floats_with_operation(v1.aFloat, v2.aFloat, op);
        case OBJ_STRING:
            return check_strings_with_operation(v1.string, v2.string, op);
        default:
            return false;
    }
}