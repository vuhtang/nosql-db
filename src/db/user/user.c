#include "db/user/user.h"

enum status user_insert_object(struct file *file, struct object *obj) {

    return file_add_entity(obj, file, 0, 0, false);
}

enum status user_read_object_property(struct file *file, struct string *path, struct query_result *res) {
    struct query q = {};
    create_query(path, &q);

    file_read(file, &q, res);

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