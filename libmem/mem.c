#include "mem.h"
#include "wrappers.h"
#include <stdint.h>

// Gcat's memory region
void *gcat_mem = NULL;
// The size of gcat's memory region
size_t gcat_size = 0;

#ifndef NO_UB
/**
 * This can compare two pointers for being greater than or equal to each other.
 * This is undefined behavior, and only works for pointers within register size,
 * as well as only working on flat architectures.
 */
#define UB_pointer_gte(ptr1, ptr2) (((uintptr_t) ptr1) >= ((uintptr_t) ptr2))

/**
 * This can compare two pointers for being less than or equal to each other.
 * This is undefined behavior, and only works for pointers within register size,
 * as well as only working on flat architectures.
 */
#define UB_pointer_lte(ptr1, ptr2) (((uintptr_t) ptr1) <= ((uintptr_t) ptr2))
#endif // NO_UB

/**
 * Calculate the size to expand GCAT's memory to, in order to ensure capacity.
 * @param size the current size of a memory block.
 * @return the new size
 */
static size_t size_ceil(size_t size, size_t bound)
{
    if (size % bound != 0)
    {
        return (size / bound) * bound + bound;
    }
    return size;
}

/**
 * Get GCAT's area in memory. If it does not exist, it is created.
 * @pre addr is in gcat's memory or null
 * @param addr the address to check
 * @return 
 */
void *get_mem(void *addr)
{
    size_t newsize = size_ceil(addr - gcat_mem, Getpagesize());
    if (gcat_mem == NULL)
    {
        gcat_mem = Mmap(NULL, Getpagesize());
    }

    if (addr == NULL)
    {
        return gcat_mem;
    }

    if ((uintptr_t) addr >= (uintptr_t) gcat_mem + gcat_size)
    {
        if (newsize > gcat_size)
        {
            gcat_size = newsize;
            gcat_mem = Mremap(gcat_mem, gcat_size, newsize);
        }
    }

    return addr;
}

/**
 * Determine if a pointer is to GCAT's managed memory.
 * @param addr the pointer to check
 * @return 1 if it is in GCAT's spaced, 0 otherwise
 */
int is_managed(void *addr)
{
    return UB_pointer_gte(addr, (void *) (gcat_mem)) &&
        UB_pointer_lte(addr, (void *) (gcat_mem + gcat_size));
}
