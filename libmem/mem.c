#include "mem.h"
#include "wrappers.h"
#include <stdint.h>

// Gcat's memory region
void *gcat_mem = NULL;
size_t length = 1ULL << 31;
// The size of gcat's memory region
void *gcat_mem_end = NULL;

// I will use 0x6CA700000000 as the base address for now
// This splits it farther than any practical system in the current day

/**
 * Get GCAT's area in memory. If it does not exist, it is created.
 * @pre addr is in gcat's memory or null
 * @param addr the address to check
 * @return 
 */
void *get_mem(void *addr)
{
    if (gcat_mem == NULL)
    {
        gcat_mem = Mmap((void *) 0x6CA700000000, length);
        gcat_mem_end = gcat_mem + length;
    }

    if (addr == NULL)
    {
        addr = gcat_mem;
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
