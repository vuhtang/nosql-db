#include "db/file/file_interaction.h"
#include "db/section/section_interaction.h"

enum status file_add_entity(struct object *obj, struct file *file, file_off ancestor_ptr, struct object *prev_sibling, bool isChild) {

    struct entity entity = entity_from_object(obj, ancestor_ptr);
    file_off addr;
    union raw_value val = obj->raw_value;
    enum status res = section_add_entity(&entity, obj->key, &val, &addr, file);
    if (res == ERROR) {
        add_new_section(file);
        res = section_add_entity(&entity, obj->key, &val, &addr, file);
        if (res == ERROR)
            return res;
    }

    obj->addr = addr;

    if (ancestor_ptr == 0 && file->header->root_object_addr == 0) {
        file->header->root_object_addr = addr;
        file->header->last_root_layer_addr = addr;
        sync_file_header(file);
    } else if (ancestor_ptr == 0) {
        section_write_sib_ptr(file->header->last_root_layer_addr, addr, file);
        file->header->last_root_layer_addr = addr;
    }

    //write ptr to curr object to prev object
    if (prev_sibling)
        section_write_sib_ptr(prev_sibling->addr, addr, file);

    if (isChild)
        section_write_child_ptr(ancestor_ptr, addr, file);

    if (obj->child) {
        res = file_add_entity(obj->child, file, addr, NULL, true);
        if (res == ERROR)
            return res;
    }

    if (obj->sibling) {
        res = file_add_entity(obj->sibling, file, obj->ancestor->addr, obj, false);
        if (res == ERROR)
            return res;
    }

    return OK;
}

enum status file_read_all(const struct query *query, struct query_result *result) {

    return OK;
}

file_off file_find_siblings_on_layer_by_key(struct file *file, struct string *key, file_off first_sibling) {
    struct entity cur_entity;
    file_off cur_ptr = first_sibling;

    while (cur_ptr != 0) {
        cur_entity = section_find_entity(file, cur_ptr);
        struct string entity_key = section_get_entity_key_by_ptr(file, cur_ptr);
        if (strings_are_equals(key, &entity_key))
            return cur_ptr;
        else
            cur_ptr = cur_entity.sibling_ptr;
    }

    return 0;
}

file_off file_find_child_by_ancestor_ptr(struct file *file, file_off ancestor_ptr) {
    struct entity ancestor_entity = section_find_entity(file, ancestor_ptr);
    return ancestor_entity.child_ptr;
}

file_off file_find_last_sibling_on_layer(struct file *file, file_off first_sibling_addr) {
    struct entity entity = section_find_entity(file, first_sibling_addr);
    if (entity.sibling_ptr != 0)
        return file_find_last_sibling_on_layer(file, entity.sibling_ptr);
    else
        return first_sibling_addr;
}

// path looks like this: "person.location.name"
// each key in its level must be unique.
// f.e. the "location" property of the "person" is unique, and the "person" object is also unique.

enum status file_find_obj_addr(struct file *file, const struct query *query, file_off *addr) {
    struct query_object_path *path = query->path;

    file_off res_obj_ptr;

    struct query_object_path_elem *cur_path_elem = path->first;
    file_off curr_obj_ptr = file->header->root_object_addr;

    // query path: "person"."location"."name"...
    // cur path elem: "person"
    struct string *key;
    while (cur_path_elem && curr_obj_ptr) {
        key = cur_path_elem->name;
        res_obj_ptr = file_find_siblings_on_layer_by_key(file, key, curr_obj_ptr);
        if (!res_obj_ptr)
            return ERROR;
        curr_obj_ptr = file_find_child_by_ancestor_ptr(file, res_obj_ptr);
        cur_path_elem = cur_path_elem->next;
    }

    if (cur_path_elem)
        return ERROR;

    *addr = res_obj_ptr;
    return OK;
}

enum status file_read(struct file *file, const struct query *query, struct query_result *result) {
    struct query_object_path *path = query->path;
    if (!path->first)
        return file_read_all(query, result);


    file_off res_obj_ptr = 0;
    enum status status = file_find_obj_addr(file, query, &res_obj_ptr);
    if (status == ERROR || res_obj_ptr == 0)
        return ERROR;

    union raw_value value;
    struct string *res_key;
    struct entity res_entity = section_find_entity_with_values(file, res_obj_ptr, &res_key, &value);

    struct object *o = object_from_entity(res_entity, res_obj_ptr, res_key, value);
    struct query_result_item i = {.object = o};
    result->first_item = &i;

    return OK;
}



file_off file_find_prev_sibling(struct file *file, struct entity *entity, file_off obj_addr) {

    if (entity_first_on_layer(file, entity, obj_addr))
        return 0;

    file_off ancestor_addr = entity->ancestor_ptr;
    struct entity first_entity;
    file_off cur_addr;
    if (!ancestor_addr) {
        first_entity = section_find_entity(file, file->header->root_object_addr);
        cur_addr = file->header->root_object_addr;
    } else {
        struct entity ancestor_entity = section_find_entity(file, ancestor_addr);
        first_entity = section_find_entity(file, ancestor_entity.child_ptr);
        cur_addr = ancestor_entity.child_ptr;
    }

    struct entity cur_entity = first_entity;
    while (cur_entity.sibling_ptr != obj_addr && cur_entity.sibling_ptr) {
        cur_addr = cur_entity.sibling_ptr;
        cur_entity = section_find_entity(file, cur_entity.sibling_ptr);
    }
    if (cur_entity.sibling_ptr != obj_addr)
        return 0;

    return cur_addr;
}

void file_delete_next_siblings(struct file *file, file_off first_obj_addr) {

    struct entity cur_entity = section_find_entity(file, first_obj_addr);
    while (cur_entity.sibling_ptr) {
        file_delete_object(file, cur_entity.sibling_ptr);
        cur_entity = section_find_entity(file, first_obj_addr);
    }
}

void delete_entity_without_sibling(struct file *file, struct entity *entity, file_off obj_addr) {

    if (entity_first_on_layer(file, entity, obj_addr)) {
        if (entity->ancestor_ptr)
            section_write_child_ptr(entity->ancestor_ptr, 0, file);
        else {
            file->header->root_object_addr = 0;
            file->header->last_root_layer_addr = 0;
            sync_file_header(file);
        }
    } else {
        file_off prev_sibling_addr = file_find_prev_sibling(file, entity, obj_addr);
        section_write_sib_ptr(prev_sibling_addr, 0, file);
        if (file->header->last_root_layer_addr == obj_addr)
            file->header->last_root_layer_addr = prev_sibling_addr;
    }
    section_delete_entity(file, obj_addr);
}

void delete_entity_with_sibling(struct file *file, struct entity *entity, file_off obj_addr) {

    if (entity_first_on_layer(file, entity, obj_addr)) {
        if (entity->ancestor_ptr)
            section_write_child_ptr(entity->ancestor_ptr, entity->sibling_ptr, file);
        else {
            file->header->root_object_addr = entity->sibling_ptr;
            sync_file_header(file);
        }

    } else {
        file_off prev_sibling_addr = file_find_prev_sibling(file, entity, obj_addr);
        section_write_sib_ptr(prev_sibling_addr, entity->sibling_ptr, file);
    }
    section_delete_entity(file, obj_addr);
}

enum status file_delete_object(struct file *file, file_off obj_addr) {

    struct entity res_entity = section_find_entity(file, obj_addr);

    if (res_entity.child_ptr) {
        file_delete_next_siblings(file, res_entity.child_ptr);
        file_delete_object(file, res_entity.child_ptr);
    }

    if (!res_entity.sibling_ptr) {
        delete_entity_without_sibling(file, &res_entity, obj_addr);
    } else {
        delete_entity_with_sibling(file, &res_entity, obj_addr);
    }

    return OK;
}

enum status file_update_obj_value(struct file *file, file_off addr, union raw_value value, enum value_type type) {

    struct entity updated_entity = section_find_entity(file, addr);
    if (updated_entity.val_type != type)
        return ERROR;

    if (type == VAL_STRING) {

        struct string *key = init_string("", updated_entity.key_size);
        union raw_value old_value = {};
        updated_entity.val_size = value.string.len;
        section_find_entity_with_values(file, addr, &key, &old_value);

        file_off prev_sibling = file_find_prev_sibling(file, &updated_entity, addr);
        bool isChild = entity_first_on_layer(file, &updated_entity, addr);

        file_off new_addr;
        section_delete_entity(file, addr);
        section_add_entity(&updated_entity, key, &value, &new_addr, file);

        if (isChild) {
            section_write_child_ptr(updated_entity.ancestor_ptr, new_addr, file);
        }

        if (prev_sibling) {
            section_write_sib_ptr(prev_sibling, new_addr, file);
        }
    } else
        section_update_entity_value(file, addr, value, type);

    return OK;
}
