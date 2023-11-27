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
    return size + bound;
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
        // I will use 0x6CA700000000 as the base address for now
        // This splits it near-infinitely far from the stack and program memory
        #define SYSTEM_DEPENDENT_MMAP_BASE_ADDR ((void *) 0x6CA700000000)
        gcat_mem = Mmap(SYSTEM_DEPENDENT_MMAP_BASE_ADDR, newsize);
        gcat_mem_end = gcat_mem + newsize;
    }

    if (addr == NULL)
    {
        addr = gcat_mem;
    }

    if (addr >= gcat_mem_end && newsize > 0)
    {
        size_t old_difference = gcat_mem_end - gcat_mem;
        gcat_mem = Mremap(gcat_mem, old_difference, gcat_mem_end - gcat_mem);
        gcat_mem_end = gcat_mem_end + newsize;
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
