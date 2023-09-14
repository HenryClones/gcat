#include <stdio.h>
#include <errno.h>

#define MAP_FAILED ((void *) -1)

static void unixerror(int n, char *stage, char *resource)
{
    fprintf(stderr,
        "GCAT error: Unix error %d during %s %s.\n", n, stage, resource);
}

int Open(char *path)
{
    int fd = open(path);
    if (fd == -1)
    {
        unixerror(errno, "initializing page by opening file path =", path);
    }
    return fd;
}

void *Mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset)
{
    void *block = mmap(addr, length, prot, flags, fd, offset);
    if (block == MAP_FAILED)
    {
        unixerror(errno, "initializing page with", "mmap function");
    }
    return block;
}
