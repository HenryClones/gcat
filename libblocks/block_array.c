#include "blocks.h"

/**
 * Get the full size of this block. This counts headers/other fragmentation.
 */
size_t block_full_size(struct block *blk)
{
    return blk->size + sizeof(struct block) - sizeof(blk->payload);
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
        blk->size < desired_size && after >= min && after < max && !get_used(after);
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
 * @return the earliest block in the line of free blocks
 */
struct block *free_block(struct block *blk, struct block *next, int has_after)
{
    if (get_finalizer(blk) != NULL)
    {
        // Execute finalizer over payload
        typedef void(* finalizer)(void *);
        ((finalizer) get_finalizer(blk))(get_payload(blk));
    }
    // Assimilate before blocks
    // Assimilate after blocks
    struct block* after = get_after(blk);
    if (has_after && !get_used(after))
    {
        set_size(blk, get_size(blk) + block_full_size(after));
    }
    struct block* before = get_before(blk);
    if (before)
    {
        set_size(before, get_size(before) + block_full_size(blk));
        blk = before;
    }
    // The block is now unused
    set_used(blk, 0, has_after);
    set_next(blk, next);
    set_prev(blk, get_next(next));
    set_size(blk, get_size(blk));
    return blk;
}

/**
 * Get the block after this one.
 * @param blk this block
 * @return the block after it
 */
struct block *get_after(struct block *blk)
{
    // Note to self: Please, in the name of every last good in each future...
    // Make sure to remember struct block pointers are not byte sized.
    // Size should be a multiple of max_align
    // size_t will fit at end of size
    return (struct block *)((uint8_t *) get_payload(blk) + get_size(blk));
}

/**
 * Get the block before this one, or NULL if it is not possible.
 * @param blk this block
 * @return the block before this one, or NULL if this cannot be done
 */
struct block *get_before(struct block *blk)
{
    if (!get_prevused(blk))
    {
        // At the end of an unused block there is a size_t
        return get_block_header((uint8_t *) blk - *((size_t *) blk - 1));
    }
    return NULL;
}
