#include "entity/entity.h"

struct entity entity_from_object(struct object *object, file_off ancestor_ptr) {
    struct entity entity = {};
    entity.key_size = object->key->len;
    if (object->type != OBJ_STRING)
        entity.val_size = sizeof(union raw_value);
    else
        entity.val_size = object->raw_value.string.len;

    switch (object->type) {
        case OBJ_BOOL:
            entity.val_type = VAL_BOOL;
            break;
        case OBJ_INT32:
            entity.val_type = VAL_INT32;
            break;
        case OBJ_FLOAT:
            entity.val_type = VAL_FLOAT;
            break;
        case OBJ_STRING:
            entity.val_type = VAL_STRING;
            break;
        case OBJ_OBJ:
            entity.val_type = VAL_OBJ;
            break;
    }

    entity.ancestor_ptr = ancestor_ptr;

    return entity;
}

enum object_type obj_type_from_ent_type(enum value_type ent_type) {
    switch (ent_type) {
        case VAL_BOOL:
            return OBJ_BOOL;
        case VAL_STRING:
            return OBJ_STRING;
        case VAL_FLOAT:
            return OBJ_FLOAT;
        case VAL_INT32:
            return OBJ_INT32;
        case VAL_OBJ:
            return OBJ_OBJ;
    }
}

struct object *object_from_entity(struct entity entity, file_off entity_addr, struct string *key, union raw_value value) {
    struct object *o = malloc(sizeof(struct object));
    o->addr = entity_addr;
    struct string *s = init_string("", key->len);
    strcpy(s->ptr, key->ptr);
    o->key = s;
    o->type = obj_type_from_ent_type(entity.val_type);
    o->raw_value = value;
    return o;
}

void free_obj(struct object *obj) {
    free_string(obj->key);
    if (obj->child) free_obj(obj->ancestor);
    if (obj->sibling) free_obj(obj->sibling);
    free(obj);
}
