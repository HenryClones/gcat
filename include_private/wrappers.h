#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#ifndef GCAT_WRAPPERS_H
#define GCAT_WRAPPERS_H

#ifndef size_t
#include <stddef.h>
#endif // size_t

void *Mmap(void *addr, size_t length);
int Getpagesize();

#endif // GCAT_WRAPPERS_H

#ifdef __cplusplus
}
#endif // __cplusplus
