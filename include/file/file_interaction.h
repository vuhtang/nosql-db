#ifndef LLP_FILE_INTERACTION_H
#define LLP_FILE_INTERACTION_H

#include "entity/entity.h"
#include "file/file.h"
#include "query/query.h"
#include "section/section_header.h"
#include "types/types.h"
#include "util/util.h"
#include <stdio.h>

enum status file_add_entity(struct object *obj, struct file *file, file_off ancestor_ptr, struct object *prev_sibling, bool isChild);
enum status file_read(struct file *file, const struct query *query, struct query_result *result);
file_off file_find_siblings_on_layer_by_key(struct file *file, struct string *key, file_off first_sibling);
enum status file_find_obj_addr(struct file *file, const struct query *query, file_off *addr);
enum status file_update_obj_value(struct file *file, file_off addr, union raw_value value, enum value_type type);
enum status file_delete_object(struct file *file, file_off obj_addr);

#endif//LLP_FILE_INTERACTION_H
