#include <sys/mman.h>
#include <sys/resource.h>
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
static int devzero_fd = -1;

// disable these functions if they do not exist
// they are just optional, performance enhancing ones
#ifndef MAP_HUGETLB
#define MAP_HUGETLB 0
#endif
#ifndef MAP_HUGE_2MB
#define MAP_HUGE_2MB 0
#endif
// Add anonymous page and hugetlb functionality
#define GCAT_MANAGED_PAGE_FLAGS (MAP_PRIVATE | MAP_HUGETLB | MAP_HUGE_2MB | MAP_ANONYMOUS)

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
static int getpagesize_cached = 0;

int Getpagesize()
{
    if (!getpagesize_cached)
    {
        #ifdef getpagesize
        getpagesize_cached = getpagesize();
        #else
        getpagesize_cached = sysconf(_SC_PAGESIZE);
        #endif
    }

    return getpagesize_cached;
}

#ifdef SECURE
/**
 * Add guard pages
 */
#define GCAT_GUARD_PAGE_PROT (PROT_NONE)
#define GCAT_GUARD_PAGE_FLAGS (MAP_FIXED | MAP_PRIVATE)
#define GUARD_PAGE_BEFORE(addr) (addr - getpagesize_cached)
#define GUARD_PAGE_AFTER(addr, size) ((addr + size) / getpagesize_cached * getpagesize_cached)
#endif

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
        if (fd == -1)
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
    #ifdef SECURE
    void *throwaway = mmap(addr - Getpagesize(), GCAT_MANAGED_PAGE_PROT,
        GCAT_MANAGED_PAGE_FLAGS, devzero_fd, 0);
    if (throwaway == MAP_FAILED)
    {
        unixerror_simple(errno, "initializing first guard page with mmap function");
    }
    void *throwaway = mmap(GUARD_PAGE_AFTER(addr, length), GCAT_GUARD_PAGE_PROT,
        GCAT_GUARD_PAGE_FLAGS, devzero_fd, 0);
    if (throwaway == MAP_FAILED)
    {
        unixerror_simple(errno, "initializing second guard page with mmap function");
    }
    #endif
    return block;
}

void *Mremap(void *addr, size_t oldlength, size_t newlength)
{
    // Set the soft memory limit
    struct rlimit *limits;
    getrlimit(RLIMIT_AS, limits);
    limits->rlim_max += newlength - oldlength;
    setrlimit(RLIMIT_AS, limits);
    // before remapping the rest of the memory
    #ifdef mremap

    // Move the guard pages
    #ifdef SECURE
    void *throwaway = mremap(GUARD_PAGE_BEFORE(addr));
    if (block == MAP_FAILED || addr != block)
    {
        unixerror_simple(errno, "moving first guard page with mremap function");
    }
    void *throwaway = mremap(addr - GUARD_PAGE_AFTER(addr, length));
    if (block == MAP_FAILED || addr != block)
    {
        unixerror_simple(errno, "moving second guard page with mremap function");
    }
    #endif

    void *block = mremap(addr, oldlength, newlength, 0);
    #else

    // Move the guard pages
    #ifdef SECURE
    void *block =  mmap(addr, newlength, GCAT_MANAGED_PAGE_PROT,
        GCAT_MANAGED_PAGE_FLAGS | MAP_FIXED, devzero_fd, 0);
    if (throwaway == MAP_FAILED || GUARD_PAGE_BEFORE(addr) != block)
    {
        unixerror_simple(errno, "expanding page with mremap function");
    }
    void *block =  mmap(addr, newlength, GCAT_MANAGED_PAGE_PROT,
        GCAT_MANAGED_PAGE_FLAGS | MAP_FIXED, devzero_fd, 0);
    if (throwaway == MAP_FAILED || GUARD_PAGE_BEFORE(addr) != throwaway)
    {
        unixerror_simple(errno, "expanding page with mremap function");
    }
    #endif

    void *block =  mmap(addr, newlength, GCAT_MANAGED_PAGE_PROT,
        GCAT_MANAGED_PAGE_FLAGS | MAP_FIXED, devzero_fd, 0);
    #endif
    if (block == MAP_FAILED || addr != block)
    {
        unixerror_simple(errno, "expanding page with mremap function");
    }
    return block;
}


int Kill(char *cause, void *address)
{
    int status = kill(-1, SIGSEGV);
    if (status == -1)
    {
        unixerror_citeobject(errno, "kill function causing SIGSEGV due to", cause, address);
    }
    return status;
}

int Kill_offset(char *cause, void *address, int offset)
{
    int status = kill(-1, SIGSEGV);
    if (status == -1)
    {
        unixerror_citeoffset(errno, "kill function causing SIGSEGV due to", cause, address, offset);
    }
    return status;
}
