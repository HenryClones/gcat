#include "mem.h"
#include "wrappers.h"

/**
 * The initial size of GCAT's garbage collected pages.
 */
#define GCAT_MANAGED_PAGE_SIZE ((size_t) 65536)

// Gcat's memory region
struct block *gcat_mem = NULL;
// The size of gcat's memory region
size_t gcat_size = GCAT_MANAGED_PAGE_SIZE;

// The last unused block by gcat
struct block *last_unused = NULL;

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

/**
 * Determine if a pointer is to GCAT's managed memory.
 * @param block the pointer to check
 * @return 1 if it is in GCAT's spaced, 0 otherwise
 */
int is_managed(void *block)
{
    return UB_pointer_gte(block, (void *) (gcat_mem->payload)) &&
        UB_pointer_lte(block, (void *) (gcat_mem + gcat_size));
}
