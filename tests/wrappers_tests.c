#include <sys/mman.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include "wrappers.h"
#include "wrappers_tests.h"

/**
 * Test wrappers.h getpagesize.
 */
int wrappers_test1()
{
    #ifdef getpagesize
    return Getpagesize() != getpagesize();
    #else
    return Getpagesize() != sysconf(_SC_PAGESIZE);
    #endif
}

/**
 * Test wrappers.h mmap.
 */
int wrappers_test2()
{
    size_t size = 65536;
    void *mem = Mmap((void *) 0x6CA700000000, size);
    // Test if it succeeds and it's not null
    // Test if it's aligned to pages
    uintptr_t mem_n = (uintptr_t) mem;
    if (mem == MAP_FAILED || mem == NULL || mem_n % 4096 != 0)
    {
        return 1;
    }
    // Write to page
    uint64_t value = 1;
    uint64_t *pos = (uint64_t *) mem;
    *pos = value;
    // Write to end of page
    size_t end = size / sizeof(uint64_t) - 1;
    pos[end] = value;
    if (pos[0] != pos[size / sizeof(uint64_t) - 1] || pos[0] != value)
    {
        return 1;
    }
    return 0;
}
