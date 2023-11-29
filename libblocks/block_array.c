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
 * @return the size_t area to place it at
 */
size_t *get_block_boundary(struct block *blk)
{
    size_t *payload = get_payload(blk);
    return payload + get_size(blk) / sizeof(size_t);
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
 * Get the true size of this block after coalescence.
 */
size_t true_size(struct block *blk)
{
    size_t size = get_size(blk);
    if (size >= sizeof(size_t) * 2)
    {
        size += *((size_t *) get_payload(blk));
    }
    return size;
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
    if (get_flag(blk) == used)
    {
        if (get_finalizer(blk) != NULL)
        {
            // Execute finalizer over payload
            typedef void(* finalizer)(void *);
            ((finalizer) get_finalizer(blk))(get_payload(blk));
        }
    }
    // The block is now unused
    set_next(blk, next);
    set_prev(blk, get_next(next));
    // Add in true size
    size_t size = 0;
    // Especially the blocks before this one
    struct block *before = get_before(blk);
    if (before)
    {
        struct block *end = get_prev(before);
        if (end)
        {
            before = end;
        }
        set_prev(blk, before);
        size += block_full_size(blk);
        blk = before;
    }
    // Size parameter to find blocks in constant time
    if (has_after)
    {
        struct block *after = get_after(blk);
        if (has_after && get_flag(after) == unused)
        {
            size += block_full_size(after);
            set_prev(after, blk);
        }
        // And reassign the correct one
    }
    // Place the size into the true size field
    if (get_size(blk) >= sizeof(size_t) * 2)
    {
        size_t *pl = get_payload(blk);
        *pl = size;
    }
    // Be unused
    set_flag(blk, unused, has_after);
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
    return (struct block *)((uint8_t *) blk + get_size(blk));
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
        return get_block_header((uint8_t *) blk - *((size_t *) blk - 1) - 0x10);
    }
    return NULL;
}
