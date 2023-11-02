void *Mmap(void *addr, size_t length);
void *Mremap(void *addr, size_t oldlength, size_t newlength);
int Kill(char *cause, void *address);
int Kill_offset(char *cause, void *address, int offset);
int Getpagesize();
