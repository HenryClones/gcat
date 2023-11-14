#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#ifndef GCAT_BLOCKS_H
#define GCAT_BLOCKS_H

#include "types.h"

#ifndef NO_UB
/**
 * This can compare two pointers for being greater than or equal to each other.
 * This is undefined behavior, and only works for pointers within register size,
 * as well as only working on flat architectures.
 */
#define UB_pointer_gte(ptr1, ptr2) (((uintptr_t) ptr1) >= ((uintptr_t) ptr2))

/**
 * This can compare two pointers for being less than or equal to each other.
 * This is undefined behavior, and only works for pointers within register size,
 * as well as only working on flat architectures.
 */
#define UB_pointer_lte(ptr1, ptr2) (((uintptr_t) ptr1) <= ((uintptr_t) ptr2))
#endif // NO_UB

int compare_refs(struct block *blk);
size_t *get_block_boundary(struct block *blk);
void set_size(struct block *blk, size_t size);
size_t get_size(struct block *blk);
void set_prev(struct block *blk, struct block *prev);
struct block *get_prev(struct block *blk);
void set_next(struct block *blk, struct block *next);
struct block *get_next(struct block *blk);
void free_block(struct block *blk);
void update_ref_total(struct block *blk, int delta);
void update_ref_strong(struct block *blk, int delta);

#endif // GCAT_BLOCKS_H

#ifdef __cplusplus
}
#endif // __cplusplus
