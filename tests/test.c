#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../include_public/gcat.h"
#include "../include_private/blocks.h"
#include "../include_private/wrappers.h"

/**
 * Select a part of gcat to test.
 */
static int select_test(char *test)
{

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
    char *string = "als;dkj";
    strcpy(fun_memory, string);
    printf("%s\n", (char*) fun_memory);
    printf("%s\n", (char*) funner_memory);
    
    // And free the blocks at the end
    burr_function((void*) 0);
    burr_function(&argc);
    burr_function(fun_memory);
    burr_function(funnest_memory);
    burr_function(funner_memory);

    return EXIT_SUCCESS;
}
