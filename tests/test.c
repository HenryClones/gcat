#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <gcat.h>
#include "galloc.h"
#include "mem.h"
#include "blocks.h"
#include "wrappers.h"

/**
 * Test galloc.h.
 */
static int galloc_test()
{
    return 0;
}

/**
 * Test blocks.h.
 */
static int blocks_test1()
{
    return 0;
}

/**
 * Test mem.h get_mem.
 */
static int mem_test1()
{
    int size = 65536;
    void *mem = get_mem(size);
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
    return 0;
}

/**
 * Test mem.h expandmem.
 */
static int mem_test2()
{
    size_t size = 65536;
    void *mem = get_mem(size);
    mem = expand_mem(mem, &size);
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
    return 0;
}

/**
 * Test wrappers.h.
 */
static int wrappers_test1()
{
    int size = 65536;
    void *mem = get_mem(size);
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
    return 0;
}

/**
 * Test wrappers.h.
 */
static int wrappers_test2()
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
    return 0;
}

/**
 * Test wrappers.h.
 */
static int wrappers_test3()
{
    size_t size = 65536;
    void *mem = Mmap(NULL, size);
    mem = Mremap(mem, size, size * 2);
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
    // Write to page
    uint64_t *pos = ((uint64_t *)mem);
    *pos = 1;
    // Write to end of page
    pos = (uint64_t *)(mem + size - sizeof(uint64_t));
    *pos = 1;
    return 0;
}

/**
 * Test gcat.h.
 */
static int gcat_test()
{
    return 0;
}

/**
 * Select a part of gcat to test.
 */
static int select_test(char *test)
{
    int results = 0;
    if (!strcmp(test, "galloc"))
    {
        results |= galloc_test();
    }

    if (!strcmp(test, "blocks") && !strcmp(test, "blocks1"))
    {
        results |= blocks_test1();
    }
    
    if (!strcmp(test, "wrappers") && !strcmp(test, "wrappers1"))
    {
        results |= wrappers_test1();
    }
    
    if (!strcmp(test, "wrappers") && !strcmp(test, "wrappers2"))
    {
        results |= wrappers_test2();
    }
    
    if (!strcmp(test, "wrappers") && !strcmp(test, "wrappers3"))
    {
        results |= wrappers_test3();
    }
    
    if (!strcmp(test, "mem") || !strcmp(test, "mem1"))
    {
        results |= mem_test1();
    }
    
    if (!strcmp(test, "mem") && !strcmp(test, "mem2"))
    {
        results |= mem_test2();
    }

    if (!strcmp(test, "gcat"))
    {
        results |= gcat_test();
    }
    
    return results;
}

/**
 * Main function which is the entrypoint for testing.
 */
int main(int argc, char **argv)
{
    if (argc > 1)
    {
        return select_test(argv[1]);
    }

    // Create some blocks
    void *fun_memory = gall(16, NULL);
    void *funner_memory = gall(48, NULL);
    void *funnest_memory = gall(32, NULL);
    // Test memory string copying integrity
    char *string = "als;dkj";
    strcpy(fun_memory, string);
    strcpy(funner_memory, string);
    printf("%s\n", (char*) fun_memory);
    printf("%s\n", (char*) funner_memory);
    
    // And unused the blocks at the end
    burr((void*) 0);
    burr(&argc);
    burr(fun_memory);
    burr(funnest_memory);
    burr(funner_memory);

    return EXIT_SUCCESS;
}
