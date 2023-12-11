#include "section/section_interaction.h"

size_t size_of_entity_with_vals(struct entity *entity) {
    return SIZE_OF_ENTITY + entity->key_size + entity->val_size;
}

enum status memcpy_or_error(void *p1, void *p2, size_t len) {
    if (memcpy(p1, p2, len) != p1) return ERROR;
    return OK;
}

file_off get_section_addr_from_entity(struct file *file, file_off entity_addr) {
    file_off first_section_addr = file->first_region->header->section_addr;
    file_off obj_section = (entity_addr - first_section_addr) / SECTION_SIZE;
    return first_section_addr + SECTION_SIZE * obj_section;
}

section_off get_sectoff_from_fileoff(file_off section_addr, file_off entity_addr) {
    return entity_addr - section_addr;
}

enum status section_add_entity(struct entity *entity, struct string *key, union raw_value *val, file_off *file_off_addr, struct file *file) {
    struct section_region *region = file->first_region;
    if (!region) return ERROR;
    while (region->header->free_space < size_of_entity_with_vals(entity))
        region = region->next;

    if (!region)
        return ERROR;

    region->header->free_space = region->header->free_space - size_of_entity_with_vals(entity) - entity->key_size - entity->val_size;

    void *p = mmap(NULL, SECTION_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, file->header->file_desc, region->header->section_addr);
    if (!p) return ERROR;

    //write key
    section_off key_addr = region->header->last_free_cell - entity->key_size;
    enum status res = memcpy_or_error(p + key_addr, key->ptr, entity->key_size);
    if (res == ERROR) return res;

    entity->key_ptr = key_addr;
    region->header->last_free_cell -= entity->key_size;

    //write val
    section_off val_addr = region->header->last_free_cell - entity->val_size;
    if (entity->val_type == VAL_STRING)
        res = memcpy_or_error(p + val_addr, val->string.ptr, entity->val_size);
    else
        res = memcpy_or_error(p + val_addr, val, key->len);
    if (res == ERROR) return res;

    entity->val_ptr = val_addr;
    region->header->last_free_cell -= entity->val_size;

    //write entity struct
    res = memcpy_or_error(p + region->header->first_free_cell, entity, sizeof(struct entity));
    if (res == ERROR) return res;

    *file_off_addr = region->header->first_free_cell + region->header->section_addr;
    region->header->first_free_cell += sizeof(struct entity);

    memcpy(p, region->header, sizeof(struct section_header));

    //unmap
    msync(p, SECTION_SIZE, MS_SYNC);
    munmap(p, SECTION_SIZE);

    return OK;
}

size_t offset_to_sib() {
    return sizeof(struct entity) - sizeof(file_off);
}

enum status section_write_sib_ptr(file_off prev_sibling_ptr, file_off cur_sibling_ptr, struct file *file) {

    file_off section_addr = get_section_addr_from_entity(file, prev_sibling_ptr);
    section_off prev_sibling_section_off = get_sectoff_from_fileoff(section_addr, prev_sibling_ptr);

    void *p = mmap(NULL, SECTION_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, file->header->file_desc, section_addr);

    enum status res = memcpy_or_error(p + prev_sibling_section_off + offset_to_sib(), &cur_sibling_ptr, sizeof(file_off));
    if (res == ERROR) return res;

    msync(p, SECTION_SIZE, MS_SYNC);
    munmap(p, SECTION_SIZE);

    return OK;
}

size_t offset_to_child() {
    return sizeof(struct entity) - 2 * sizeof(file_off);
}

enum status section_write_child_ptr(file_off ancestor_ptr, file_off child_ptr, struct file *file) {

    file_off section_addr = get_section_addr_from_entity(file, ancestor_ptr);
    section_off ancestor_section_off = get_sectoff_from_fileoff(section_addr, ancestor_ptr);

    void *p = mmap(NULL, SECTION_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, file->header->file_desc, section_addr);

    enum status res = memcpy_or_error(p + ancestor_section_off + offset_to_child(), &child_ptr, sizeof(file_off));
    if (res == ERROR) return res;

    msync(p, SECTION_SIZE, MS_SYNC);
    munmap(p, SECTION_SIZE);

    return OK;
}

struct entity section_find_entity(struct file *file, file_off addr) {
    struct entity entity = {};

    file_off section_addr = get_section_addr_from_entity(file, addr);
    section_off entity_sectoff = get_sectoff_from_fileoff(section_addr, addr);

    void *p = mmap(NULL, SECTION_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, file->header->file_desc, section_addr);

    memcpy_or_error(&entity, p + entity_sectoff, SIZE_OF_ENTITY);

    munmap(p, SECTION_SIZE);

    return entity;
}

struct entity section_find_entity_with_values(struct file *file, file_off entity_addr, struct string **key, union raw_value *value) {
    struct entity entity = {};

    file_off section_addr = get_section_addr_from_entity(file, entity_addr);
    section_off entity_sectoff = get_sectoff_from_fileoff(section_addr, entity_addr);

    void *p = mmap(NULL, SECTION_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, file->header->file_desc, section_addr);

    memcpy(&entity, p + entity_sectoff, SIZE_OF_ENTITY);

    //read key
    char c[entity.key_size];
    memcpy(c, p + entity.key_ptr, entity.key_size);
    *key = init_string(entity.key_size);
    strcpy((*key)->ptr, c);

    //read val

    if (entity.val_type != VAL_OBJ && entity.val_type != VAL_STRING)
        memcpy(value, p + entity.val_ptr, entity.val_size);
    else if (entity.val_type == VAL_STRING) {
        char c1[entity.val_size];
        memcpy(c1, p + entity.val_ptr, entity.val_size);
        struct string *key1 = init_string(entity.val_size);
        strcpy(key1->ptr, c1);
        value->string = *key1;
        //        value->string = init_string(entity.val_size);
    } else
        value = NULL;

    munmap(p, SECTION_SIZE);

    return entity;
}
struct string section_get_entity_key_by_ptr(struct file *file, file_off addr) {
    struct string key = {};
    section_off key_addr;
    size_t key_size;

    file_off section_addr = get_section_addr_from_entity(file, addr);
    section_off entity_sectoff = get_sectoff_from_fileoff(section_addr, addr);

    void *p = mmap(NULL, SECTION_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, file->header->file_desc, section_addr);

    memcpy(&key_addr, p + entity_sectoff, sizeof(key_addr));
    memcpy(&key_size, p + entity_sectoff + sizeof(key_addr), sizeof(key_size));

    char c[key_size];
    memcpy(c, p + key_addr, key_size);

    munmap(p, SECTION_SIZE);

    key.len = key_size;
    key.ptr = c;

    return key;
}

section_off offset_to_val_ptr() {
    return sizeof(section_off) + sizeof(size_t);
}

void section_update_entity_value(struct file *file, file_off entity_addr, union raw_value value, enum value_type type) {

    if (type == VAL_STRING)
        return;// :)))

    file_off section_addr = get_section_addr_from_entity(file, entity_addr);
    section_off entity_sectoff = get_sectoff_from_fileoff(section_addr, entity_addr);
    section_off val_ptr;

    void *p = mmap(NULL, SECTION_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, file->header->file_desc, section_addr);

    memcpy(&val_ptr, p + entity_sectoff + offset_to_val_ptr(), sizeof(section_off));
    memcpy(p + val_ptr, &value, sizeof(value));

    msync(p, SECTION_SIZE, MS_SYNC);
    munmap(p, SECTION_SIZE);
}

bool entityFirstOnLayer(struct file *file, struct entity *entity, file_off entity_addr) {
    file_off ancestor_ptr = entity->ancestor_ptr;
    if (!ancestor_ptr)
        return file->header->root_object_addr == entity_addr;

    struct entity entity_ancestor = {};
    file_off section_addr = get_section_addr_from_entity(file, ancestor_ptr);
    section_off entity_sectoff = get_sectoff_from_fileoff(section_addr, ancestor_ptr);

    void *p = mmap(NULL, SECTION_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, file->header->file_desc, section_addr);

    memcpy(&entity_ancestor, p + entity_sectoff, sizeof(entity_ancestor));

    msync(p, SECTION_SIZE, MS_SYNC);
    munmap(p, SECTION_SIZE);

    return entity_ancestor.child_ptr == entity_addr;
}

void section_delete_entity(struct file *file, file_off entity_addr) {
    file_off section_addr = get_section_addr_from_entity(file, entity_addr);
    section_off entity_sectoff = get_sectoff_from_fileoff(section_addr, entity_addr);
    struct entity entity = {};

    struct section_header header = {};

    void *p = mmap(NULL, SECTION_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, file->header->file_desc, section_addr);

    memcpy(&header, p, sizeof(header));
    memcpy(&entity, p + entity_sectoff, sizeof(struct entity));

    size_t sz = sizeof(struct entity);
    void *tmp = malloc(sz);
    memset(tmp, 0, sz);
    memcpy(p + entity_sectoff, tmp, sz);
    free(tmp);

    sz = entity.key_size;
    tmp = malloc(sz);
    memset(tmp, 0, sz);
    memcpy(p + entity.key_ptr, tmp, sz);
    free(tmp);

    sz = entity.val_size;
    tmp = malloc(sz);
    memset(tmp, 0, sz);
    memcpy(p + entity.val_ptr, tmp, sz);
    free(tmp);

    section_off last_entity_addr = header.first_free_cell - sizeof(struct entity);
    if (last_entity_addr == entity_sectoff) {
        header.first_free_cell -= sizeof(struct entity);
        header.last_free_cell += entity.key_size + entity.val_size;
        header.free_space += header.last_free_cell - header.first_free_cell;
    }

    //sync header
    memcpy(p, &header, sizeof(header));

    msync(p, SECTION_SIZE, MS_SYNC);
    munmap(p, SECTION_SIZE);

    sync_section_header_in_file_struct(file, &header);
}
