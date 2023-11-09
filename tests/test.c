#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <gcat.h>
#include "blocks.h"
#include "wrappers.h"

/**
 * Select a part of gcat to test.
 */
static int select_test(char *test)
{

    if (strcmp(test, "blocks"))
    {
        return 0;
    }
    
    if (strcmp(test, "wrappers"))
    {
        return 0;
    }
    
    if (strcmp(test, "mem"))
    {
        return 0;
    }

    if (strcmp(test, "gcat"))
    {
        return 0;
    }
    
    return 1;
}

/**
 * Main function which is the entrypoint for testing.
 */
int main(int argc, char **argv)
{
    if (argc > 1)
    {
        return select_test(argv[EXIT_FAILURE]);
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
