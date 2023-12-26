#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "da.h"
#include "da_rand.h"
#include "dbgprint.h"

/**
 * Initializes the dynamic array. `element_size` refers to the `sizeof(type_t)`. `initial_capacity` is
 * total size of the data array (i.e. `_capacity` times `element_size`).
 */
int da_initialize(struct dynamic_array* da, size_t initial_capacity, size_t element_size)
{
    da->_data = calloc(initial_capacity, element_size);
    if (da->_data == NULL)
    {
        return 1;
    }

    da->_count = 0;
    da->_capacity = initial_capacity;
    da->_element_size = element_size;
    da->default_expand_amount = 8;

    return 0;
}

/**
 * Frees the dynamic array and sets the `_data` member to `NULL`.
 */
int da_free(struct dynamic_array* da)
{
    if (da->_data == NULL)
    {
        return 1;
    }

    free(da->_data);
    da->_count = 0;
    da->_capacity = 0;
    da->_element_size = 0;

    return 0;
}

static void* da_retrieve(struct dynamic_array* da, size_t index)
{
    if (da->_data == NULL)
    {
        return NULL;
    }

    return (char*)da->_data + (da->_element_size * index);
}

/**
 * Returns the element at `index` as a `void*`. Be sure to cast return value.
 */
void* da_at(struct dynamic_array* da, size_t index)
{
    if (da->_data == NULL)
    {
        return NULL;
    }

    if (index >= da->_count)
    {
        return NULL;
    }

    return da_retrieve(da, index);
}

/**
 * Expands dynamic array by the given amount. The amount shall be based on
 * the element size, not the size in bytes.
 */
int da_expand(struct dynamic_array* da, size_t expand_amount)
{
    if (da->_data == NULL)
    {
        return 1;
    }

    void* temp_ptr;
    temp_ptr = realloc(da->_data, (da->_capacity * da->_element_size) + (expand_amount * da->_element_size));

    if (temp_ptr == NULL)
    {
        return 2;
    }

    da->_data = temp_ptr;
    da->_capacity += expand_amount;

    return 0;
}

/**
 * Shrinks dynamic array to `_count`, freeing unused memory.
 */
int da_shrink(struct dynamic_array* da)
{
    if (da->_data == NULL)
    {
        return 1;
    }

    if (da->_count == 0)
    {
        return 2;
    }

    size_t new_capacity = da->_count;
    void* temp_ptr;
    temp_ptr = realloc(da->_data, new_capacity * da->_element_size);

    if (temp_ptr == NULL)
    {
        return 3;
    }

    da->_data = temp_ptr;
    da->_capacity = new_capacity;

    return 0;
}

/**
 * Adds given element to the end of the dynamic array. This will automatically
 * expand the dynamic array if necessary based on the `default_expand_amount` member.
 */
int da_push(struct dynamic_array* da, void* element)
{
    if (da->_data == NULL)
    {
        return 1;
    }

    if (da->_count >= da->_capacity)
    {
        if (da_expand(da, da->default_expand_amount))
        {
            return 2;
        }
    }

    void* temp_ptr;
    temp_ptr = da_retrieve(da, da->_count);

    if (memcpy(temp_ptr, element, da->_element_size) == NULL)
    {
        // ???
        return 3;
    }

    ++da->_count;

    return 0;
}

/**
 * Removes the last element from the dynamic array.
 */
int da_pop(struct dynamic_array* da)
{
    if (da->_data == NULL)
    {
        return 1;
    }

    --da->_count;

    return 0;
}

int da_insert(struct dynamic_array* dest, struct dynamic_array* insrt, size_t pos)
{
    if (dest->_data == NULL)
    {
        return 1;
    }

    if (insrt->_data == NULL)
    {
        return 2;
    }

    if (dest->_element_size != insrt->_element_size)
    {
        return 3;
    }

    // If requested position is greater than the array size
    // set position to the end
    if (pos >= dest->_count)
    {
        pos = dest->_count;
    }

    void* first_partition = NULL;
    void* second_partition = NULL;
    void* combined_array = NULL;

    size_t size_fp = 0;
    size_t size_sp = 0;
    size_t size_ip = 0;
    size_t current_bpos = 0; // current position in bytes
    size_t new_count;
    new_count = dest->_count + insrt->_count;

    combined_array = malloc(new_count * dest->_element_size);

    if (combined_array == NULL)
    {
        return 4;
    }

    // Copy first partition into combined array
    // if requested position allows for it
    if (pos > 0)
    {
        size_fp = pos * dest->_element_size;
        first_partition = dest->_data;
        current_bpos += size_fp;
        memcpy(combined_array, first_partition, size_fp);
    }

    // Copy insert array into combined array
    size_ip = insrt->_count * dest->_element_size;
    memcpy((char*)combined_array + current_bpos, insrt->_data, size_ip);

    // Copy second partition into combined array
    // if requested position allows for it
    if (pos < dest->_count)
    {
        size_sp = (dest->_count - pos) * dest->_element_size;
        second_partition = (char*)dest->_data + size_fp;
        current_bpos += size_ip;
        memcpy((char*)combined_array + current_bpos, second_partition, size_sp);
    }

    free(dest->_data);
    dest->_data = combined_array;
    dest->_count = new_count;
    dest->_capacity = new_count;

    return 0;
}

int da_shuffle(struct dynamic_array* da)
{
    if (da->_data == NULL)
    {
        return 1;
    }

    void* temp_copy;
    void* temp_ptr1;
    void* temp_ptr2;
    temp_copy = malloc(da->_element_size);
    size_t i;
    for (i = 0; i < da->_count; ++i)
    {
        temp_ptr1 = da_at(da, i);
        memcpy(temp_copy, temp_ptr1, da->_element_size);
        temp_ptr2 = da_at(da, da_rand64() % da->_count);
        memcpy(temp_ptr1, temp_ptr2, da->_element_size);
        memcpy(temp_ptr2, temp_copy, da->_element_size);
    }

    free(temp_copy);

    return 0;
}

int da_sort(struct dynamic_array* da, size_t count, size_t offset, int (*compar)(const void*, const void*))
{
    if (da->_data == NULL)
    {
        return 1;
    }

    qsort((char*)da->_data + (offset * da->_element_size), count, da->_element_size, compar);

    return 0;
}