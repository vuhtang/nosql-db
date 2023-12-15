#ifndef LLP_SECTION_INTERACTION_H
#define LLP_SECTION_INTERACTION_H

#include "entity/entity.h"
#include "section/section_header.h"

#define SIZE_OF_ENTITY sizeof(struct entity)

enum status section_add_entity(struct entity *entity, struct string *key, union raw_value *val, file_off *file_off_addr, struct file *file);

enum status section_write_sib_ptr(file_off prev_sibling_ptr, file_off sibling_ptr, struct file *file);
enum status section_write_child_ptr(file_off ancestor_ptr, file_off child_ptr, struct file *file);

struct entity section_find_entity(struct file *file, file_off addr);
struct string section_get_entity_key_by_ptr(struct file *file, file_off addr);
struct entity section_find_entity_with_values(struct file *file, file_off entity_addr, struct string **key, union raw_value *value);
void section_update_entity_value(struct file *file, file_off entity_addr, union raw_value value, enum value_type type);

bool entity_first_on_layer(struct file *file, struct entity *entity, file_off entity_addr);
void section_delete_entity(struct file *file, file_off entity_addr);


#endif
