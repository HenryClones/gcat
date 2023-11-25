#include <sys/mman.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include "wrappers.h"
#include "wrappers_tests.h"

/**
 * Test wrappers.h.
 */
int wrappers_test1()
{
    return Getpagesize() != getpagesize();
}

/**
 * Test wrappers.h.
 */
int wrappers_test2()
{
    size_t size = 65536;
    void *mem = Mmap(NULL, size);
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
    pos = (uint64_t *)(mem + size - sizeof(uint64_t));
    *pos = 1;
    munmap(mem, size);
    return 0;
}

/**
 * Test wrappers.h.
 */
int wrappers_test3()
{
    size_t size = 65536;
    void *mem = Mmap(NULL, size);
    char *memc = mem;
    memc[0] = 'a';
    void *mem2 = Mremap(mem, size, size * 2);
    size *= 2;
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
    // Test if it's the same
    if (mem != mem2)
    {
        return EXIT_FAILURE;
    }
    // Test if it's the exact same
    if (memc[0] != 'a')
    {
        return EXIT_FAILURE;
    }
    // Write to page
    uint64_t *pos = ((uint64_t *)mem);
    *pos = 1;
    // Write to end of page
    pos = (uint64_t *)(mem + size - sizeof(uint64_t));
    *pos = 1;
    munmap(mem, size);
    return 0;
}
