#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#ifndef size_t
#include <stddef.h>
#endif // size_t

void *Mmap(void *addr, size_t length);
void *Mremap(void *addr, size_t oldlength, size_t newlength);
int Kill(char *cause, void *address);
int Kill_offset(char *cause, void *address, int offset);
int Getpagesize();

#ifdef __cplusplus
}
#endif // __cplusplus
