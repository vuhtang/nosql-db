#include "util/util.h"

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
    if (len == 0)
        strcpy(s->ptr, c);
    return s;
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