#ifndef LLP_USER_H
#define LLP_USER_H

#include "file/file_interaction.h"
#include "query/query.h"

enum status user_insert_object(struct file *file, struct object *obj);
enum status user_read_object_property(struct file *file, struct string *path, struct query_result *res);
enum status user_modify_object_property(struct file *file, struct string *path, union raw_value value, enum query_value_type type);
enum status user_delete_object(struct file *file, struct string *path);

#endif