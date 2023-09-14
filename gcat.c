#include <pthread.h>
#include <sys/mman.h>
#include <wrappers.h>
#include <gcat.h>

/*
Header size as an offset.
*/
#define HEADER_SIZE 0x20
/*
The memory segment can be read and written to, but not executed.
It should be protected as it grows upward in the memory when needed.
*/
#define GCAT_MANAGED_PAGE_PROT (PROT_READ | PROT_WRITE | PROT_GROWSUP)
/*
Mapped page settings for the created memory segment.
*/
#define GCAT_MANAGED_PAGE_FLAGS (MAP_SHARED)

static void *gcat_mem = NULL;

/**
 * Initialize the block of memory used for dynamic allocation,
 * as well as prerequisites for garbage collection.
 */
static void init_gc()
{
    int fd = Open("/dev/zero");
    gcat_mem = Mmap(NULL, GCAT_MANAGED_PAGE_SIZE, GCAT_MANAGED_PAGE_PROT,
        GCAT_MANAGED_PAGE_FLAGS, fd, 0);
    if (gcat_mem == MAP_FAILED)
    {
        
    }
}

/**
 * GCAT's customized garbage-collected memory allocator.
 * @return The memory which was allocated, or NULL if it failed.
 */
void *gall()
{
    // Initialize gcat_mem if it does not exist
    if (!gcat_mem)
    {
        init_gc();
    }
    return gcat_mem + HEADER_SIZE;
}
