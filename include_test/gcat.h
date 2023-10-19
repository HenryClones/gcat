#ifndef GCAT_MANAGED_PAGE_SIZE
#define GCAT_MANAGED_PAGE_SIZE 65536
#endif

typedef void(* reaper)(void *);
void *gall(int size, reaper destructor);
void burr(void*);
