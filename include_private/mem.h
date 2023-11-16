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
int is_managed(void *block);

/**
 * The initial size of GCAT's garbage collected pages.
 */
#define GCAT_MANAGED_PAGE_SIZE ((size_t) 65536)

#endif // GCAT_MEM_H

#ifdef __cplusplus
}
#endif // __cplusplus
