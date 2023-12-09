#include <sys/mman.h>
#include <sys/resource.h>
#include <stdint.h>
#include <stdio.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "../include_private/wrappers.h"

/**
 * The memory segment can be read and written to, but not executed.
 * It should be protected as it grows upward in the memory when needed.
 */
#define GCAT_MANAGED_PAGE_PROT (PROT_READ | PROT_WRITE)
#define GCAT_GUARD_PAGE_PROT (PROT_NONE)

#ifdef MAP_ANONYMOUS
#define GCAT_MANAGED_PAGE_FLAGS (MAP_PRIVATE /* | MAP_HUGETLB | MAP_HUGE_2MB */ | MAP_ANONYMOUS)
#define GCAT_GUARD_PAGE_FLAGS (MAP_PRIVATE | MAP_FIXED | MAP_ANONYMOUS)
#else
#define GCAT_MANAGED_PAGE_FLAGS (MAP_PRIVATE /* | MAP_HUGETLB | MAP_HUGE_2MB */)
#define GCAT_GUARD_PAGE_FLAGS (MAP_PRIVATE | MAP_FIXED)
#endif

#ifndef MAP_FAILED
#define MAP_FAILED ((void *) -1)
#endif

// Mimic/implement anonymous pages
static int devzero_fd = -1;

// One string, given by the offending function.
static void unixerror_simple(int n, char *cause)
{
    fprintf(stderr,
        "GCAT error: Unix error %d during %s.\n", n, cause);
}

// Cache the results of the below function
#define GETPAGESIZE_CACHED_UNSET 0
static int getpagesize_cached = GETPAGESIZE_CACHED_UNSET;

/**
 * Get the page size of this system.
 * @return the system page size
 */
int Getpagesize()
{
    if (getpagesize_cached == GETPAGESIZE_CACHED_UNSET)
    {
        #ifdef getpagesize
        getpagesize_cached = getpagesize();
        #else
        getpagesize_cached = sysconf(_SC_PAGESIZE);
        #endif
    }

    return getpagesize_cached;
}

/**
 * Get the position of the first guard page.
 */
static void *guard_page_before_position(void *base)
{
    return base - Getpagesize();
}

/**
 * Create the first guard page.
 */
static void create_guard_page_before(void *addr)
{
    void *throwaway = mmap(guard_page_before_position(addr),
        1, GCAT_GUARD_PAGE_PROT, GCAT_GUARD_PAGE_FLAGS, devzero_fd, 0);
    if (throwaway == MAP_FAILED)
    {
        unixerror_simple(errno, "initializing first guard page with mmap function");
    }
}

// First guard page should stay at the first address

/**
 * Get the position of the second guard page.
 */
static void *guard_page_after_position(void *base, size_t size)
{
    return (base + size) - ((uintptr_t) (base + size) % (uintptr_t) Getpagesize());
}

/**
 * Create the second guard page.
 */
static void create_guard_page_after(void *addr, size_t length)
{
    void *throwaway = mmap(guard_page_after_position(addr, length),
        1, GCAT_GUARD_PAGE_PROT, GCAT_GUARD_PAGE_FLAGS, devzero_fd, 0);
    if (throwaway == MAP_FAILED)
    {
        unixerror_simple(errno, "initializing second guard page with mmap function");
    }
}

void *Mmap(void *addr, size_t length)
{
    #ifndef MAP_ANONYMOUS
    // Create the /dev/zero file descriptor
    if (devzero_fd == -1)
    {
        devzero_fd = open("/dev/zero", O_RDWR);
        if (!devzero_fd)
        {
            unixerror_simple(errno, "initializing page by opening file path /dev/zero");
        }
    }
    #endif // MAP_ANONYMOUS

    void *block = mmap(addr, length, GCAT_MANAGED_PAGE_PROT,
        GCAT_MANAGED_PAGE_FLAGS, devzero_fd, 0);
    // Fallback
    if (block == MAP_FAILED)
    {
        block = mmap(NULL, length, GCAT_MANAGED_PAGE_PROT,
            GCAT_MANAGED_PAGE_FLAGS, devzero_fd, 0);
    }
    if (block == MAP_FAILED)
    {
        unixerror_simple(errno, "initializing page with mmap function");
    }
    // Add the guard page creation
    create_guard_page_before(block);
    create_guard_page_after(block, length);
    return block;
}
