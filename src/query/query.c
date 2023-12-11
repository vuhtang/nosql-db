#include "query/query.h"

struct query_result *init_query_result() {
    return malloc(sizeof(struct query_result));
}

struct query_result_item *init_query_result_item() {
    return malloc(sizeof(struct query_result_item));
}

void add_ope_to_op(struct query_object_path *op, char *ptr) {
    struct string *str = init_string(strlen(ptr));
    strcpy(str->ptr, ptr);

    struct query_object_path_elem *ope = malloc(sizeof(struct query_object_path_elem));
    ope->name = str;
    if (!op->first)
        op->first = ope;
    else {
        struct query_object_path_elem *cur = op->first;
        while (cur->next) cur = cur->next;
        cur->next = ope;
    }
}

void create_query(struct string *path, struct query *q) {
    char *delimiter = ".";
    char path1[path->len];
    strcpy(path1, path->ptr);
    struct query_object_path *op = malloc(sizeof(struct query_object_path));

    char *token = strtok(path1, delimiter);
    while (token) {
        add_ope_to_op(op, token);
        token = strtok(NULL, delimiter);
    }
    q->path = op;
}

void free_query_object_path(struct query *q) {
    struct query_object_path_elem *elem = q->path->first;
    while (elem) {
        struct query_object_path_elem *tmp = elem->next;
        free_string(elem->name);
        free(elem);
        elem = tmp;
    }
    free(q->path);
}

void create_query_condition(union raw_value value, enum query_value_type type, struct query_condition *condition, enum query_value_cmp_op op) {
    struct query_condition_element *element = malloc(sizeof(struct query_condition_element));
    element->value_type = type;
    element->value = value;
    element->op = op;
    condition->condition = element;
}

void free_query_condition(struct query_condition *condition) {
    struct query_condition *elem = condition;
    while (elem) {
        struct query_condition *tmp = elem->next;
        free_string(elem->condition->key);
        free(elem->condition);
        free(elem);
        elem = tmp;
    }
    free(condition);
}