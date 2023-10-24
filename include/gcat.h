#ifndef GCAT_MANAGED_PAGE_SIZE
#define GCAT_MANAGED_PAGE_SIZE 65536
#endif

#ifndef size_t
#include <stddef.h>
#endif

typedef void(* reaper)(void *);
void burr(void *pointer);
void *bounds_checked_access(void *pointer, int base, int offset, int step);
void *hew_function(void *pointer);
void *hew_object(void *pointer);
void *gall(size_t size, reaper destructor);
void burr_function(void *pointer);
void burr_object(void *pointer);
