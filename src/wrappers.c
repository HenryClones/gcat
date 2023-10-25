#define _SYSTEMV_SOURCE
#include <sys/mman.h>
#include <stdio.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

/**
 * The memory segment can be read and written to, but not executed.
 * It should be protected as it grows upward in the memory when needed.
 */
#define GCAT_MANAGED_PAGE_PROT (PROT_READ | PROT_WRITE | PROT_GROWSUP)

/**
 * Mapped page settings for the created memory segment.
 */
#define GCAT_MANAGED_PAGE_FLAGS (MAP_SHARED)

#define MAP_FAILED ((void *) -1)

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

int Open(char *path)
{
    int fd = open(path, O_RDWR);
    if (fd == -1)
    {
        unixerror_citereason(errno, "initializing page by opening file path ", path);
    }
    return fd;
}

int Close(int fd, char *name)
{
    int result = close(fd);
    if (result == -1)
    {
        unixerror_citereason(errno, "after initializing page closing ", name);
    }
    return fd;
}

void *Mmap(void *addr, size_t length, int fd)
{
    void *block = mmap(addr, length, GCAT_MANAGED_PAGE_PROT,
        GCAT_MANAGED_PAGE_FLAGS, fd, 0);
    if (block == MAP_FAILED)
    {
        unixerror_simple(errno, "initializing page with mmap function");
    }
    return block;
}

void *Mremap(void *addr, size_t oldlength, size_t newlength)
{
    #ifdef mremap
    void *block = mremap(addr, oldlength, newlength, 0);
    #else
    int fd = Open("/dev/zero");
    void *block =  mmap(addr, newlength, GCAT_MANAGED_PAGE_PROT,
        GCAT_MANAGED_PAGE_FLAGS | MAP_FIXED, fd, 0);
    Close(fd, "/dev/zero");
    #endif
    if (block == MAP_FAILED)
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
