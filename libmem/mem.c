#include "mem.h"
#include "wrappers.h"

/**
 * Calculate the size to expand GCAT's memory to, in order to ensure capacity.
 * @param size the current size of a memory block.
 * @return the new size
 */
static size_t get_newsize(size_t *size)
{
    *size *= 2;
    return *size;
}

/**
 * Grow GCAT's memory.
 * @param newsize the new size of GCAT'S memory
 */
static void *grow_mem(void *mem, size_t size, size_t newsize)
{
    return Mremap(mem, size, newsize);
}

/**
 * Expand the memory afforded to GCAT.
 */
void *expand_mem(void *mem, size_t *size)
{
    int current_size = *size;
    return grow_mem(mem, current_size, get_newsize(size));
}

/**
 * Get a page of memory for GCAT.
 * @return the new page of memory
 */
void *get_mem(size_t size)
{
    return Mmap(NULL, size);
}
