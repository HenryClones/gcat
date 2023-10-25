int Open(char *path);
int Close(int fd, char *name);
void *Mmap(void *addr, size_t length, int fd);
void *Mremap(void *addr, size_t oldlength, size_t newlength);
int Kill(char *cause, void *address);
int Kill_offset(char *cause, void *address, int offset);
