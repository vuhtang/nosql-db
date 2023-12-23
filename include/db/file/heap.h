#ifndef LLP_HEAP_H
#define LLP_HEAP_H

#include "db/section/section_header.h"
#include "stdlib.h"
#include "db/types/types.h"

struct heap_elem {
    struct section_region *region;
};

struct sections_heap {
    size_t capacity;
    size_t size;
    struct heap_elem *arr;
};

void heap_insert(struct sections_heap *h, struct heap_elem elem);
struct heap_elem heap_get_max(struct sections_heap *h);
struct heap_elem heap_elem_from_region(struct section_region *region);
void free_heap_arr(struct heap_elem *arr, size_t len);
void free_heap_elem(struct heap_elem *elem);
void heap_update_elem(struct sections_heap *heap, struct section_header *header);

#endif
