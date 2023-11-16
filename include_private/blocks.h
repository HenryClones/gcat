#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#ifndef GCAT_BLOCKS_H
#define GCAT_BLOCKS_H

#include "types.h"

#define VARIABLE_LENGTH_ARRAY 1
// Handle every type of block
struct block
{
    // The size of a block.
    size_t size;
    // Flag data associated with a block
    struct
    {
        // coalescence: unused -> !prev_unused
        liberty unused;
        liberty prev_unused;
    } flags;

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
            gcat_reaper finalizer;
        } used_block;
    } header;

    // The payload, offsetof must work here
    // uint64_t forces alignment on 64-bit systems for now
    // Ends with the size
    uint8_t payload[VARIABLE_LENGTH_ARRAY];
} __attribute__((aligned));

size_t *get_block_boundary(struct block *blk);
void set_flag(struct block *blk, liberty new, int has_next);
liberty get_flag(struct block *blk);
liberty get_prevflag(struct block *blk);
void set_size(struct block *blk, size_t size);
size_t get_size(struct block *blk);
void set_prev(struct block *blk, struct block *prev);
struct block *get_prev(struct block *blk);
void set_next(struct block *blk, struct block *next);
struct block *get_next(struct block *blk);
int compare_refs(struct block *blk);
void update_ref_total(struct block *blk, int delta);
void update_ref_strong(struct block *blk, int delta);
void *get_payload(struct block *blk);
void free_block(struct block *blk);
struct block *get_after(struct block *blk);
struct block *get_before(struct block *blk);

#endif // GCAT_BLOCKS_H

#ifdef __cplusplus
}
#endif // __cplusplus
