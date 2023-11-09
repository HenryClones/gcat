#include "types.h"

void coalesce(struct block *blk);
size_t get_newsize(size_t size);
inline struct block *get_unused(size_t size);
void grow_mem(size_t newsize);
void get_page();
void init_mem();
void expand_mem();
inline int is_managed(void *block);
struct block *get_block_header(void *pointer);
void make_block(struct block *position, struct block *prev, struct block *next,
    liberty is_unused, size_t block_size, gcat_reaper finalizer);