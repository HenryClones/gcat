#ifndef GCAT_MANAGED_PAGE_SIZE
#define GCAT_MANAGED_PAGE_SIZE 65536
#endif

void *gall(int size);
void *grow(void*, int size);
void burr(void*);
