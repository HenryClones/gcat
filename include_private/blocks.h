#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#ifndef GCAT_BLOCKS_H
#define GCAT_BLOCKS_H

#include "types.h"
#include <stdint.h>

#define VARIABLE_LENGTH_ARRAY 1
// Handle every type of block
struct block
{
    // The size of a block.
    size_t size;
    // Flag data associated with a block
    block_flags flags;

    union
    {
        struct
        {
            // The pointers in the explicit unused list.
            struct
            {
                // treat as implementation dependent?
                struct block *prev;
                struct block *next;
            } pointers;
        } unused_block;

        struct
        {
            // A block's users are stored as a unit.
            struct
            {
                // treat as implementation dependent?
                uint32_t total_users;
                uint32_t strong_users;
            } users;
            // The finalizer, if defined
            void(* finalizer)(void *);
        } used_block;
    } header;

    // The payload, offsetof must work here
    // uint64_t forces alignment on 64-bit systems for now
    // Ends with the size
    uint8_t payload[VARIABLE_LENGTH_ARRAY] __attribute__((aligned));
} __attribute__((aligned));

// block_properties.c
void init_flags(struct block *blk);
void set_used(struct block *blk, int new, int has_next);
void set_prevused(struct block *blk, int new);
int get_used(struct block *blk);
int get_prevused(struct block *blk);
void set_size(struct block *blk, size_t size);
size_t get_size(struct block *blk);
void set_prev(struct block *blk, struct block *prev);
struct block *get_prev(struct block *blk);
void set_next(struct block *blk, struct block *next);
struct block *get_next(struct block *blk);
int get_ref_total(struct block *blk);
void set_ref_total(struct block *blk, int x);
int get_ref_strong(struct block *blk);
void set_ref_strong(struct block *blk, int x);
void *get_payload(struct block *blk);
void set_finalizer(struct block *blk, void(* finalizer)(void *));
void *get_finalizer(struct block *blk);

// block_array.c
size_t block_full_size(struct block *blk);
size_t *get_block_boundary(struct block *blk);
struct block *get_after(struct block *blk);
struct block *get_before(struct block *blk);
struct block *coalesce(struct block *min, struct block *max, struct block *blk, size_t desired_size);
size_t true_size(struct block *blk);
struct block *free_block(struct block *blk, struct block *next, int has_after);
struct block * __attribute__ ((const)) get_block_header(void *position);

#endif // GCAT_BLOCKS_H

#ifdef __cplusplus
}
#endif // __cplusplus
