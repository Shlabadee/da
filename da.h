#ifndef __ArHPpZcTmq
#define __ArHPpZcTmq

#include <stddef.h>

struct dynamic_array
{
    size_t _count;
    size_t _capacity;
    size_t _element_size;

    size_t default_expand_amount;

    void* _data;
};

int da_initialize(struct dynamic_array* da, size_t initial_capacity, size_t element_size);
int da_free(struct dynamic_array* da);
void* da_at(struct dynamic_array* da, size_t index);
int da_expand(struct dynamic_array* da, size_t expand_amount);
int da_shrink(struct dynamic_array* da);
int da_push(struct dynamic_array* da, void* element);
int da_pop(struct dynamic_array* da);
int da_insert(struct dynamic_array* dest, struct dynamic_array* src, size_t pos);
int da_shuffle(struct dynamic_array* da);
int da_sort(struct dynamic_array* da, size_t count, size_t offset, int (*compar)(const void*, const void*));

#endif