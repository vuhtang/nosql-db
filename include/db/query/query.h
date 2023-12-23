#ifndef LLP_QUERY_H
#define LLP_QUERY_H

#include "stdlib.h"
#include "string.h"
#include "db/types/types.h"
#include "db/util/util.h"

struct query_object_path_elem {
    struct string *name;
    struct query_object_path_elem *next;
};

struct query_object_path {
    struct query_object_path_elem *first;
};

struct query_condition_element {
    struct string *key;// field to compare with field of certain object
    enum query_value_type value_type;
    union raw_value value;     // value of this field
    enum query_value_cmp_op op;// comparison operation
};

struct query_condition {
    struct query_condition_element *condition;
    struct query_condition *next;
};

struct query {
    struct query_object_path *path;   // path to certain object
    struct query_condition *condition;// multiple conditions on object's properties
};                                    // so the query is "go there and take it if it fits conditions"

struct query_result_item {
    struct object *object;
    struct query_result_item *next;
};

struct query_result {
    struct query_result_item *first_item;
};

void create_query(struct string *path, struct query *q);
void free_query_object_path(struct query *q);
void create_query_condition(union raw_value value, enum query_value_type type, struct query_condition *condition, enum query_value_cmp_op op);

#endif//LLP_QUERY_H
