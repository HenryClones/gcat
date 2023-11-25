#include "blocks.h"

/**
 * Get the full size of this block. This counts headers/other fragmentation.
 */
size_t block_full_size(struct block *blk)
{
    return blk->size + sizeof(struct block) - sizeof(blk->payload);
}

/**
 * Block boundary.
 * @pure
 * @param blk the block
 * @param size the size for where the boundary is
 * @return the size_t area to place it at
 */
size_t *get_block_boundary(struct block *blk)
{
    size_t *payload = (size_t*) blk->payload;
    size_t payload_offset = get_size(blk) / (sizeof(size_t) / sizeof(*(blk->payload)));
    return payload + payload_offset;
}

/**
 * Coalesce all unused blocks around this one in the best way possible.
 * @pre blk is unused
 * @post blk may not be valid and is agglomerated as a larger unused block
 */
struct block *coalesce(struct block *min, struct block *max, struct block *blk, size_t desired_size)
{
    // Blocks after this one
    struct block *after;
    for (after = get_after(blk);
        blk->size < desired_size && after >= min && after < max && get_flag(after) == unused;
        after = get_after(blk))
    {
        blk->size += block_full_size(after);
    }

    // Blocks before this one
    struct block *before;
    for (before = get_before(blk);
        blk->size < desired_size && before != NULL && before >= min && before < max;
        blk = before, before = get_before(blk))
    {
        before->size += blk->size;
    }

    return blk;
}

/**
 * Free a block. next != NULL
 * @pre block is used and has no users and last_unused != NULL
 * @post block will be freed up and coalesced
 * @param next the next block in the chain, likely the last free one, or blk
 */
struct block *free_block(struct block *blk, struct block *next, int has_after)
{
    if (blk->header.used_block.finalizer != NULL)
    {
        // Execute finalizer over payload
        typedef void(* finalizer)(void *);
        ((finalizer) get_finalizer(blk))(blk->payload);
    }
    // The block is now unused
    set_prev(blk, get_prev(next));
    set_next(blk, next);
    // Be unused
    set_flag(blk, unused, has_after);
    set_size(blk, get_size(blk));
    // And reassign the correct one
    return blk;
}

/**
 * Get the block after this one.
 * @param blk this block
 * @return the block after it
 */
struct block *get_after(struct block *blk)
{
    int size = get_size(blk);
    // Size should be a multiple of max_align
    // size_t will fit at end of size
    return blk + block_full_size(blk);
}

/**
 * Get the block before this one, or NULL if it is not possible.
 * @param blk this block
 * @return the block before this one, or NULL if this cannot be done
 */
struct block *get_before(struct block *blk)
{
    if (get_prevflag(blk) == unused)
    {
        // At the end of an unused block there is a size_t
        size_t *boundary = ((size_t *) blk) - 1;
        return blk + *boundary;
    }
    return NULL;
}