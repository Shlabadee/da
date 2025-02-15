#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

#include <stddef.h>

typedef struct DynamicArray
{
	size_t _count;
	size_t _capacity;
	size_t _element_size;

	size_t default_expand_amount;

	void* _data;
} DynamicArray;

int da_initialize(DynamicArray* da, size_t initial_capacity, size_t element_size);
int da_free(DynamicArray* da);
void* da_at(DynamicArray* da, size_t index);
int da_expand(DynamicArray* da, size_t expand_amount);
int da_shrink(DynamicArray* da);
int da_push(DynamicArray* da, void* element);
int da_pop(DynamicArray* da);
int da_insert(DynamicArray* dest, DynamicArray* src, size_t pos);
int da_sort(DynamicArray* da, size_t count, size_t offset,
            int (*compar)(const void*, const void*));

#endif
