#include "db/user/user.h"

enum object_type object_type_from_query_value_type(enum query_value_type type);

enum status user_insert_object(struct file *file, struct object *obj) {

    return file_add_entity(obj, file, 0, 0, false);
}

struct query_object_path_elem *delete_last_query_elem(struct query q) {
    struct query_object_path_elem *cur = q.path->first;
    struct query_object_path_elem *prev = NULL;
    while (cur->next) {
        prev = cur;
        cur = cur->next;
    }
    if (!prev)
        q.path->first = NULL;
    else
        prev->next = NULL;

    return cur;
}

enum status user_insert_object_with_path(struct file *file, struct object *obj, struct string *path) {
    struct query q = {};
    create_query(path, &q);
    struct query_object_path_elem *last_elem = delete_last_query_elem(q);
    struct query_result res = {};
    file_read(file, &q, &res);

    if (!res.first_item) {
        obj->key = last_elem->name;
        file_add_entity(obj, file, 0, 0, false);
        return OK;
    }

    struct object *o = res.first_item->object;
    file_off ancestor_ptr = o->addr;

    obj->key = last_elem->name;

    file_off child_ptr = file_find_child_by_ancestor_ptr(file, ancestor_ptr);
    if (child_ptr != 0) {
        file_off last_sib = file_find_last_sibling_on_layer(file, child_ptr);
        struct object o1 = {.addr = last_sib};
        return file_add_entity(obj, file, ancestor_ptr, &o1, false);
    } else {
        return file_add_entity(obj, file, ancestor_ptr, 0, true);
    }
}

enum status user_read_object_property(struct file *file, struct string *path, struct query_result *res) {
    struct query q = {};
    create_query(path, &q);

    file_read(file, &q, res);

    free_query_object_path(&q);
    return OK;
}

enum status user_read_object_property_with_condition(struct file *file, struct string *path, struct query_condition *condition, struct query_result *res) {
    struct query q = {};
    create_query(path, &q);

    file_read(file, &q, res);

    struct query_result res1 = {};
    struct query_condition *cur = condition;
    while (cur) {
        struct query_condition_element *element = cur->condition;
        struct string *path1 = string_concat_with_dot(path, element->key);
        user_read_object_property(file, path1, &res1);

        struct object *property = res1.first_item->object;
        if (property->type == object_type_from_query_value_type(element->value_type)) {
            if (!check_values_with_operation(property->raw_value, element->value, property->type, element->op)) {
                return ERROR;
            }
        }

        cur = cur->next;
    }
    free_query_object_path(&q);
    return OK;
}

enum value_type value_type_from_query_value_type(enum query_value_type type) {
    switch (type) {
        case Q_VAL_BOOL:
            return VAL_BOOL;
        case Q_VAL_FLOAT:
            return VAL_FLOAT;
        case Q_VAL_INT32:
            return VAL_INT32;
        case Q_VAL_STRING:
            return VAL_STRING;
        default:
            return VAL_BOOL;
    }
}

enum object_type object_type_from_query_value_type(enum query_value_type type) {
    switch (type) {
        case Q_VAL_BOOL:
            return OBJ_BOOL;
        case Q_VAL_FLOAT:
            return OBJ_FLOAT;
        case Q_VAL_INT32:
            return OBJ_INT32;
        default:
            return OBJ_STRING;
    }
}

enum status user_modify_object_property(struct file *file, struct string *path, union raw_value value, enum query_value_type type) {
    struct query q = {};
    create_query(path, &q);

    struct query_condition condition = {};
    create_query_condition(value, type, &condition, SET_VAL);

    file_off obj_addr = -1;
    enum status status = file_find_obj_addr(file, &q, &obj_addr);
    if (status == -1 || obj_addr == -1)
        return ERROR;

    enum value_type val_type = value_type_from_query_value_type(type);
    file_update_obj_value(file, obj_addr, value, val_type);

    return OK;
}

enum status user_delete_object(struct file *file, struct string *path) {
    struct query q = {};
    create_query(path, &q);

    file_off obj_addr = 0;
    enum status status = file_find_obj_addr(file, &q, &obj_addr);
    if (status == ERROR || obj_addr == 0)
        return ERROR;

    file_delete_object(file, obj_addr);

    return OK;
}