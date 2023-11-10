#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#ifndef GCAT_GCAT_H
#define GCAT_GCAT_H

#ifndef size_t
#include <stddef.h>
#endif // size_t

void *bounds_checked_access(void *pointer, int base, int offset, int step);
void *hew_function(void *pointer);
void *hew_object(void *pointer);
void *gall(size_t size, void *finalizer);
void burr(void *pointer);
// void burr_object(void *pointer);

#endif // GCAT_GCAT_H

#ifdef __cplusplus
}
#endif // __cplusplus
