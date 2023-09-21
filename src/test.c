#include <gcat.h>
#include <assert.h>

/**
 * Main function which is the entrypoint for testing.
 */
int main(int argc, char **argv)
{
    void* fun_memory = gall(16);
    void* funner_memory = gall(48);
    void* funnest_memory = gall(32);
    burr((void*) 0);
    burr(&argc);
    burr(fun_memory);
    burr(funnest_memory);
    burr(funner_memory);
}
