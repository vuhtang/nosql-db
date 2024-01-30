#ifndef LLP_USER_H
#define LLP_USER_H

#include "db/file/file_interaction.h"
#include "db/query/query.h"
#include "db/section/section_interaction.h"

enum status user_insert_object(struct file *file, struct object *obj);
enum status user_insert_object_with_path(struct file *file, struct object *obj, struct string *path);
enum status user_read_object_property(struct file *file, struct string *path, struct query_result *res);
enum status user_read_object_property_with_condition(struct file *file, struct string *path, struct query_condition *condition, struct query_result *res);
enum status user_modify_object_property(struct file *file, struct string *path, union raw_value value, enum query_value_type type);
enum status user_delete_object(struct file *file, struct string *path);

#endif