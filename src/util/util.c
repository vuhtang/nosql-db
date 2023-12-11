#include "util/util.h"

bool strings_are_equals(struct string *s1, struct string *s2) {
    if (s1->len != s2->len)
        return false;
    return strncmp(s1->ptr, s2->ptr, s1->len) == 0;
}

struct string *init_string(size_t len) {
    char *ptr = malloc(sizeof(char) * len);
    struct string *s = malloc(sizeof(struct string));
    s->ptr = ptr;
    s->len = len;
    return s;
}

void free_string(struct string *str) {
    free(str->ptr);
    free(str);
}

void print_status(enum status s) {
    s == OK ? printf("OK\n") : printf("ERROR\n");
}