#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "da.h"

// clang-format off
#ifdef DEBUG
    #include <stdio.h>
    #define DBGPRINT printf
#else
    #define DBGPRINT(...) do {} while(0)
#endif
// clang-format on

static bool h_memlap(const void* ptr1, size_t size1, const void* ptr2, size_t size2)
{
	const char* start1 = (const char*)ptr1;
	const char* end1 = start1 + size1;
	const char* start2 = (const char*)ptr2;
	const char* end2 = start2 + size2;
	return (start1 < end2) && (start2 < end1);
}

/**
 * Initializes the dynamic array. `element_size` refers to the `sizeof(type_t)`.
 * `initial_capacity` is total size of the data array (i.e. `_capacity` times `element_size`).
 */
int da_initialize(DynamicArray* da, size_t initial_capacity, size_t element_size)
{
	da->_data = calloc(initial_capacity, element_size);
	if (da->_data == NULL) return DA_ERROR_DATA_NULL;

	da->_count = 0;
	da->_capacity = initial_capacity;
	da->_element_size = element_size;
	da->default_expand_amount = 8;

	return 0;
}

/**
 * Frees the dynamic array and sets the `_data` member to `NULL`.
 */
int da_free(DynamicArray* da)
{
	if (da->_data == NULL) return DA_ERROR_DATA_NULL;

	free(da->_data);
	da->_data = NULL;
	da->_count = 0;
	da->_capacity = 0;
	da->_element_size = 0;

	return 0;
}

static void* h_da_retrieve(DynamicArray* da, size_t index)
{
	if (da->_data == NULL) return NULL;

	return (char*)da->_data + (da->_element_size * index);
}

/**
 * Returns the element at `index` as a `void*`. Be sure to cast return value.
 */
void* da_at(DynamicArray* da, size_t index)
{
	if (da->_data == NULL) return NULL;

	if (index >= da->_count) return NULL;

	return h_da_retrieve(da, index);
}

/**
 * Expands dynamic array by the given amount. The amount shall be based on
 * the element size, not the size in bytes.
 */
int da_expand(DynamicArray* da, size_t expand_amount)
{
	if (da->_data == NULL) return DA_ERROR_DATA_NULL;

	void* temp_ptr;
	temp_ptr = realloc(da->_data, (da->_capacity * da->_element_size)
									  + (expand_amount * da->_element_size));

	if (temp_ptr == NULL) return DA_ERROR_DATA_EXPANSION_FAILED;

	da->_data = temp_ptr;
	da->_capacity += expand_amount;

	return 0;
}

/**
 * Shrinks dynamic array to `_count`, freeing unused memory.
 */
int da_shrink(DynamicArray* da)
{
	if (da->_data == NULL) return DA_ERROR_DATA_NULL;

	if (da->_count == 0)
	{
		free(da->_data);
		da->_data = NULL;
		da->_capacity = 0;
		return DA_WARNING_COUNT_ZERO;
	}

	// Shrink to exact size
	size_t new_capacity = da->_count;
	void* temp_ptr = realloc(da->_data, new_capacity * da->_element_size);

	// Only update on success
	if (temp_ptr != NULL)
	{
		da->_data = temp_ptr;
		da->_capacity = new_capacity;
	}

	return temp_ptr ? 0 : DA_ERROR_DATA_SHRINK_FAILED;
}

/**
 * Adds given element to the end of the dynamic array. This will automatically
 * expand the dynamic array if necessary based on the `default_expand_amount` member.
 */
int da_push(DynamicArray* da, void* element)
{
	if (da->_data == NULL) return DA_ERROR_DATA_NULL;

	if (da->_count >= da->_capacity)
	{
		int error;
		if (error = da_expand(da, da->default_expand_amount)) return error;
	}

	void* temp_ptr;
	temp_ptr = h_da_retrieve(da, da->_count);

	if (memcpy(temp_ptr, element, da->_element_size) == NULL)
		return DA_ERROR_UNKNOWN; // A weird error

	++da->_count;

	return 0;
}

/**
 * Removes the last element from the dynamic array.
 */
int da_pop(DynamicArray* da)
{
	if (da->_data == NULL) return DA_ERROR_DATA_NULL;

	if (da->_count == 0) return DA_WARNING_COUNT_ZERO;

	--da->_count;

	return 0;
}

int da_insert(DynamicArray* dest, DynamicArray* src, size_t pos)
{
	if (pos > dest->_count) return DA_ERROR_INVALID_POSITION;

	// Detect memory overlap
	if (h_memlap(dest->_data, (dest->_count * dest->_element_size), src->_data,
				 (src->_count * src->_element_size)))
		return DA_ERROR_OVERLAPPING_MEMORY;

	// Expand if necessary
	if ((dest->_count + src->_count) > dest->_capacity)
	{
		int error = da_expand(dest, src->_count);
		if (error) return error;
	}

	// Perform memory move and insert
	memmove(((char*)dest->_data + ((pos + src->_count) * dest->_element_size)),
			((char*)dest->_data + (pos * dest->_element_size)),
			((dest->_count - pos) * dest->_element_size));

	memcpy(((char*)dest->_data + (pos * dest->_element_size)), src->_data,
		   (src->_count * src->_element_size));

	dest->_count += src->_count;
	return 0;
}

int da_sort(DynamicArray* da, size_t count, size_t offset,
			int (*compar)(const void*, const void*))
{
	if (da->_data == NULL) return DA_ERROR_DATA_NULL;

	qsort((char*)da->_data + (offset * da->_element_size), count, da->_element_size, compar);

	return 0;
}
