#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#ifndef GCAT_MEM_H
#define GCAT_MEM_H

#ifndef size_t
#include <stddef.h>
#endif

void *expand_mem(void *mem, size_t *size);
void *get_mem(size_t size);

#endif // GCAT_MEM_H

#ifdef __cplusplus
}
#endif // __cplusplus
