#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#ifndef __GCAT_H__
#define __GCAT_H__

#define GCAT_MANAGED_PAGE_SIZE 65536

#ifndef size_t
#include <stddef.h>
#endif // size_t

typedef void(* reaper)(void *);
void *bounds_checked_access(void *pointer, int base, int offset, int step);
void *hew_function(void *pointer);
void *hew_object(void *pointer);
void *gall(size_t size, reaper finalizer);
void burr_function(void *pointer);
// void burr_object(void *pointer);

#endif // __GCAT_H__

#ifdef __cplusplus
}
#endif // __cplusplus
