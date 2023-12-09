#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#ifndef GCAT_GCAT_H
#define GCAT_GCAT_H

#ifndef size_t
#include <stddef.h>
#endif // size_t

void *bounds_checked_access(void *pointer, size_t base, size_t offset, size_t step);
void *gall(size_t size, void(* finalizer)(void *));
void *hew_stack(void *pointer);
void *hew_heap(void *pointer);
void burr_stack(void *pointer);
void burr_heap(void *pointer);

#endif // GCAT_GCAT_H

#ifdef __cplusplus
}
#endif // __cplusplus
