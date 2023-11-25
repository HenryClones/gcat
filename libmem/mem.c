#include "mem.h"
#include "wrappers.h"
#include <stdint.h>

// Gcat's memory region
void *gcat_mem = NULL;
// The size of gcat's memory region
void *gcat_mem_end = NULL;

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
    size_t newsize = size_ceil(addr - gcat_mem_end, Getpagesize());
    if (gcat_mem == NULL)
    {
        gcat_mem = Mmap(NULL, Getpagesize());
        gcat_mem_end = gcat_mem + Getpagesize();
    }

    if (addr == NULL)
    {
        addr = gcat_mem;
    }

    if (addr >= gcat_mem_end && newsize > 0)
    {
        size_t old_difference = gcat_mem_end - gcat_mem;
        gcat_mem_end += newsize;
        gcat_mem = Mremap(gcat_mem, old_difference, gcat_mem_end - gcat_mem);
    }

    return addr;
}

/**
 * Determine if a pointer is to GCAT's managed memory.
 * @param addr the pointer to check
 * @return 1 if it is in GCAT's spaced, 0 otherwise
 */
int __attribute__((pure)) is_managed(void *addr)
{
    return addr >= gcat_mem && addr < gcat_mem_end;
}
