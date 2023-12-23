#include "db/file/heap.h"

bool compare_sections(struct heap_elem s, struct heap_elem compare_to_s) {
    return s.region->header->free_space > compare_to_s.region->header->free_space;
}

void heap_sift_down(struct sections_heap *h, size_t index) {
    size_t left = index * 2 + 1;
    size_t right = index * 2 + 2;
    size_t max = index;

    if (left >= h->size)
        left = -1;
    if (right >= h->size)
        right = -1;

    if (right != -1 && left != -1 && compare_sections(h->arr[right], h->arr[left]))
        max = right;
    else if (right != -1 && compare_sections(h->arr[right], h->arr[index]))
        max = right;
    else if (left != -1 && compare_sections(h->arr[left], h->arr[index]))
        max = left;

    if (max != index) {
        struct heap_elem temp = h->arr[max];
        h->arr[max] = h->arr[index];
        h->arr[index] = temp;

        heap_sift_down(h, max);
    }
}

struct heap_elem heap_get_max(struct sections_heap *h) {
    struct heap_elem elem = {};
    if (h->size == 0) {
        return elem;
    }

    elem = h->arr[0];

    return elem;
}

void heap_sift_up(struct sections_heap *h, size_t index) {

    if (!index)
        return;

    size_t parent = (index - 1) / 2;

    if (compare_sections(h->arr[index], h->arr[parent])) {

        struct heap_elem temp = h->arr[parent];
        h->arr[parent] = h->arr[index];
        h->arr[index] = temp;

        heap_sift_up(h, parent);
    }
}

void heap_expand(struct sections_heap *h) {

    struct heap_elem *p = malloc(sizeof(struct heap_elem) * h->capacity * 2);
    memcpy(p, h->arr, sizeof(struct heap_elem) * h->size);
    free(h->arr);
    h->arr = p;
    h->capacity = h->capacity * 2;
}

void heap_insert(struct sections_heap *h, struct heap_elem elem) {

    if (h->size >= h->capacity)
        heap_expand(h);

    h->arr[h->size] = elem;
    heap_sift_up(h, h->size);
    h->size++;
}

void heap_update_elem(struct sections_heap *heap, struct section_header *header) {
    size_t index = -1;
    for (size_t i = 0; i < heap->size; ++i) {
        if (heap->arr[i].region->header->section_addr == header->section_addr)
            index = i;
    }

    if (index == -1)
        return;

    size_t old_free_space = heap->arr[index].region->header->free_space;
    heap->arr[index].region->header->free_space = header->free_space;
    heap->arr[index].region->header->first_free_cell = header->first_free_cell;
    heap->arr[index].region->header->last_free_cell = header->last_free_cell;

    if (old_free_space >= header->free_space)
        heap_sift_down(heap, index);
    else
        heap_sift_up(heap, index);
}

struct heap_elem heap_elem_from_region(struct section_region *region) {
    struct heap_elem elem = {};
    elem.region = region;
    return elem;
}

void free_heap_arr(struct heap_elem *arr, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        struct heap_elem elem = arr[i];
        free_heap_elem(&arr[i]);
    }
    free(arr);
}

void free_heap_elem(struct heap_elem *elem) {
    free_section_region(elem->region);
}
