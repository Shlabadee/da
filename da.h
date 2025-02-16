#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

#define DA_ERROR_DATA_NULL 1
#define DA_ERROR_DATA_EXPANSION_FAILED 2
#define DA_WARNING_COUNT_ZERO 0xf2
#define DA_ERROR_DATA_SHRINK_FAILED 3
#define DA_ERROR_OVERLAPPING_MEMORY 4
#define DA_ERROR_INVALID_POSITION
#define DA_ERROR_UNKNOWN 0xf

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

#ifdef __cplusplus
}
#endif

#endif
