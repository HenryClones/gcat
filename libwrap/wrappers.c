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
#define GCAT_MANAGED_PAGE_PROT (PROT_READ | PROT_WRITE | PROT_GROWSUP)

#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS 0
#endif
// Otherwise, fully mimic anonymous pages
static int devzero_fd = 0;

#define GCAT_MANAGED_PAGE_FLAGS (MAP_PRIVATE | /* MAP_HUGETLB | MAP_HUGE_2MB | */ MAP_ANONYMOUS)

#define GCAT_GUARD_PAGE_PROT (PROT_NONE)
#define GCAT_GUARD_PAGE_FLAGS (MAP_FIXED | MAP_PRIVATE)

#ifndef MAP_FAILED
#define MAP_FAILED ((void *) -1)
#endif

// One string, given by the offending function.
static void unixerror_simple(int n, char *cause)
{
    fprintf(stderr,
        "GCAT error: Unix error %d during %s.\n", n, cause);
}

// Two strings, the stage/function and the resource.
static void unixerror_citereason(int n, char *stage, char *resource)
{
    fprintf(stderr,
        "GCAT error: Unix error %d during %s %s.\n", n, stage, resource);
}

// Two strings and a pointer, the causing function, the resource, and the offending pointer.
static void unixerror_citeobject(int n, char *stage, char *resource, void *blame)
{
    fprintf(stderr,
        "GCAT error: Unix error %d during %s %s with %p.\n", n, stage, resource, blame);
}

// Two strings and a pointer, the causing function, the resource, and the offending pointer.
static void unixerror_citeoffset(int n, char *stage, char *resource, void *blame, int offset)
{
    fprintf(stderr,
        "GCAT error: Unix error %d during %s %s at %p:%d.\n", n, stage, resource, blame, offset);
}

// Cache the results of the below function
#define GETPAGESIZE_CACHED_UNSET 0
static int getpagesize_cached = GETPAGESIZE_CACHED_UNSET;

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
    #ifdef MAP_ANONYMOUS
    void *throwaway = mmap(guard_page_before_position(addr),
        1, GCAT_GUARD_PAGE_PROT, GCAT_GUARD_PAGE_FLAGS | MAP_ANONYMOUS, -1, 0);
    #else
    void *throwaway = mmap(guard_page_before_position(addr),
        1, GCAT_GUARD_PAGE_PROT, GCAT_GUARD_PAGE_FLAGS, devzero_fd, 0);
    #endif // MAP_ANONYMOUS
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
    #ifdef MAP_ANONYMOUS
    void *throwaway = mmap(guard_page_after_position(addr, length),
        1, GCAT_GUARD_PAGE_PROT, GCAT_GUARD_PAGE_FLAGS | MAP_ANONYMOUS, -1, 0);
    #else
    void *throwaway = mmap(guard_page_after_position(addr, length),
        1, GCAT_GUARD_PAGE_PROT, GCAT_GUARD_PAGE_FLAGS, devzero_fd, 0);
    #endif // MAP_ANONYMOUS
    if (throwaway == MAP_FAILED)
    {
        unixerror_simple(errno, "initializing second guard page with mmap function");
    }
}

/**
 * Move the second guard page.
 */
static void move_guard_page_after(void *addr, size_t old_length, size_t new_length)
{
    // mremap is linux exclusive!
    #ifdef mremap
    // Move the guard pages
    void *throwaway = mremap(guard_page_after_position(addr, old_length), guard_page_before_position(addr, new_length));
    #else

    #ifdef MAP_ANONYMOUS
    void *throwaway = mmap(guard_page_after_position(addr, old_length),
        1, GCAT_GUARD_PAGE_PROT, GCAT_GUARD_PAGE_FLAGS | MAP_ANONYMOUS, -1, 0);
    #else
    void *throwaway = mmap(guard_page_after_position(addr, old_length),
        1, GCAT_GUARD_PAGE_PROT, GCAT_GUARD_PAGE_FLAGS, devzero_fd, 0);
    #endif // MAP_ANONYMOUS
    
    #endif // mremap

    if (throwaway == MAP_FAILED || addr != throwaway)
    {
        unixerror_simple(errno, "moving second guard page with mremap function");
    }
}

void *Mmap(void *addr, size_t length)
{
    // Set the soft memory limit
    struct rlimit limits;
    getrlimit(RLIMIT_AS, &limits);
    limits.rlim_max += length;
    setrlimit(RLIMIT_AS, &limits);
    #ifndef MAP_ANONYMOUS
    // Create the /dev/zero file descriptor
    if (!devzero_fd)
    {
        devzero_fd = open("/dev/zero", O_RDWR);
        if (!devzero_fd)
        {
            unixerror_simple(errno, "initializing page by opening file path /dev/zero");
        }
        fd = devzero_fd;
    }
    #endif

    void *block = mmap(addr, length, GCAT_MANAGED_PAGE_PROT,
        GCAT_MANAGED_PAGE_FLAGS, devzero_fd, 0);
    if (block == MAP_FAILED)
    {
        unixerror_simple(errno, "initializing page with mmap function");
    }
    // Add the guard page creation
    create_guard_page_before(addr);
    create_guard_page_after(addr, length);
    return block;
}

void *Mremap(void *addr, size_t old_length, size_t new_length)
{
    // Set the soft memory limit
    struct rlimit limits;
    getrlimit(RLIMIT_AS, &limits);
    limits.rlim_max += new_length - old_length;
    setrlimit(RLIMIT_AS, &limits);
    
    // finish with guard pages
    move_guard_page_after(addr, old_length, new_length);

    // before remapping the rest of the memory
    #ifdef mremap
    void *block = mremap(addr, old_length, new_length, 0);
    #else
    void *block =  mmap(addr, new_length, GCAT_MANAGED_PAGE_PROT,
        GCAT_MANAGED_PAGE_FLAGS | MAP_FIXED, devzero_fd, 0);
    #endif

    if (block == MAP_FAILED || addr != block)
    {
        unixerror_simple(errno, "expanding page with mremap function");
    }
    return block;
}
