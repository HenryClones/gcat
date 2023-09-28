#include <gcat.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

/**
 * Main function which is the entrypoint for testing.
 */
int main(int argc, char **argv)
{
    void *fun_memory = gall(16);
    void *funner_memory = gall(48);
    void *funnest_memory = gall(32);
    char *string = "als;dkj";
    strcpy(fun_memory, string);
    printf("%s", (char*) fun_memory);
    printf("%s", (char*) funner_memory);
    
    burr((void*) 0);
    burr(&argc);
    burr(fun_memory);
    burr(funnest_memory);
    burr(funner_memory);
    
    printf("\n");
}
