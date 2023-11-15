#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#ifndef GCAT_GALLOC_H
#define GCAT_GALLOC_H

#include "blocks.h"

void init_mem();
void coalesce(struct block *blk);
struct block *get_unused(size_t size);
struct block *get_block_header(void *pointer);
void make_block(struct block *position, struct block *prev, struct block *next,
    liberty is_unused, size_t block_size, gcat_reaper finalizer);

#endif // GCAT_GALLOC_H

#ifdef __cplusplus
}
#endif // __cplusplus
