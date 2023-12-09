#include <sys/mman.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include "mem.h"
#include "mem_tests.h"

/**
 * Test mem.h get_mem.
 */
int mem_test1()
{
    void *mem = get_mem(NULL);
    size_t size = getpagesize();
    // Test if it succeeds
    if (mem == MAP_FAILED)
    {
        return EXIT_FAILURE;
    }
    // Test if it's not null
    if (mem == NULL)
    {
        return EXIT_FAILURE;
    }
    // Test if it's aligned to pages
    if ((long long int)(mem) % getpagesize() != 0)
    {
        return EXIT_FAILURE;
    }
    // Write to page
    uint64_t *pos = ((uint64_t *)mem);
    *pos = 1;
    // Write to end of page
    pos = (uint64_t *) get_mem(mem + size - sizeof(uint64_t));
    *pos = 1;
    return 0;
}

/**
 * Test mem.h expandmem.
 */
int mem_test2()
{
    void *mem = get_mem(NULL);
    size_t size = 65536;
    void *end = get_mem(mem + size);
    // Test if it succeeds
    if (mem == MAP_FAILED)
    {
        return EXIT_FAILURE;
    }
    // Test if it's not null
    if (mem == NULL)
    {
        return EXIT_FAILURE;
    }
    // Test if it's aligned to pages
    if ((long long int)(mem) % getpagesize() != 0)
    {
        return EXIT_FAILURE;
    }
    // Write to page
    uint64_t *pos = ((uint64_t *)mem);
    *pos = 1;
    // Write to end of page
    pos = (uint64_t *)(end - sizeof(uint64_t));
    *pos = 1;
    return 0;
}
