#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#ifndef GCAT_MEM_H
#define GCAT_MEM_H

#ifndef size_t
#include <stddef.h>
#endif

void *get_mem(void *addr);
int __attribute__ ((const)) is_managed(void *block);

#endif // GCAT_MEM_H

#ifdef __cplusplus
}
#endif // __cplusplus
