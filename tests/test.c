#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <gcat.h>
#include "blocks_tests.h"
#include "mem_tests.h"
#include "wrappers_tests.h"
#include "gcat_tests.h"
#include "galloc_tests.h"

/**
 * Select a part of gcat to test.
 */
static int select_test(char *test)
{
    int results = 0;
    if (!strcmp(test, "galloc"))
    {
        results |= galloc_test1();
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
        results |= gcat_test1();
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
