#include <sys/mman.h>
#include <stdio.h>
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

static void unixerror(int n, char *stage, char *resource)
{
    fprintf(stderr,
        "GCAT error: Unix error %d during %s %s.\n", n, stage, resource);
}

int Open(char *path)
{
    int fd = open(path, O_RDWR);
    if (fd == -1)
    {
        unixerror(errno, "initializing page by opening file path =", path);
    }
    return fd;
}

int Close(int fd)
{
    int result = close(fd);
    if (result == -1)
    {
        unixerror(errno, "after initializing page closing ", "/dev/zero");
    }
    return fd;
}

void *Mmap(void *addr, size_t length, int fd)
{
    void *block = mmap(addr, length, GCAT_MANAGED_PAGE_PROT,
        GCAT_MANAGED_PAGE_FLAGS, fd, 0);
    if (block == MAP_FAILED)
    {
        unixerror(errno, "initializing page with", "mmap function");
    }
    return block;
}
