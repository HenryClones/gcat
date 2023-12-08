#include "blocks.h"
#include "mem.h"
#include "galloc.h"

#define INITIAL_SIZE 1 << 24

// The last unused block by gcat
struct block *last_unused = NULL;

/**
 * Initialize the first area in memory if ptr == null, otherwise fallthrough
 */
static struct block *find_mem(struct block *ptr)
{
    // Initialize first block
    if (ptr == NULL)
    {
        ptr = get_mem(ptr);
        init_flags(ptr);
        set_size(ptr, INITIAL_SIZE);
        set_finalizer(ptr, NULL);
        free_block(ptr, ptr, 0);
    }
    return ptr;
}

/**
 * Get the next unused block above a certain size.
 * @pre there is at least one unused block in gcat_mem
 * @param size the size of the block to get
 * @return a position to the next unused block.
 */
void *get_unused(size_t size)
{
    // initialize last_unused
    // walk down the chain and grab the first free block's payload
    struct block *position;
    for (position = get_mem(last_unused = find_mem(last_unused));
        get_size(position) < size && get_next(position) != last_unused;
        position = get_mem(get_next(position)));
    get_mem(position + size);
    return get_payload(position);
}

/**
 * Use a block, splitting extra space off to the right.
 * 
 */
void *use_block(void *block, void (*finalizer)(void *), size_t size)
{
    // Get this block and its siblings
    struct block *blk = get_block_header(block);
    struct block *next = get_next(blk);
    struct block *prev = get_prev(blk);

    // Get the padding to create a free block after this one
    size_t padding = block_full_size(blk) - size;

    // If the padding is big enough to create a new block
    if (padding >= sizeof(struct block))
    {
        set_size(blk, size);
        struct block *after = get_after(blk);
        set_used(blk, 1, 1);
        // Make the padding into a block payload size
        padding -= sizeof(struct block);
        // Then make that free block
        set_used(after, 0, is_managed(get_after(after)));
        set_size(after, padding);
        // Set last_unused
        last_unused = after;
        set_next(last_unused, next);
        set_prev(last_unused, prev);
    }
    // Otherwise, add the padding to the size so it can fit
    else
    {
        size += padding;
        set_size(blk, size);
        set_used(blk, 1, is_managed(blk));
        // Set it here, too
        if (next == blk)
        {
            last_unused = next;
            set_prev(last_unused, prev);
        }
        else if (prev == blk)
        {
            last_unused = prev;
            set_next(last_unused, next);
        }
        else
        {
            last_unused = NULL;
        }
        last_unused = blk;
    }

    // Then, finish setting the block as used
    set_finalizer(blk, finalizer);
    set_ref_strong(blk, 1);

    return get_payload(blk);
}

/**
 * Free a struct block.
 * @param position the block at a position
 */
void make_block_free(void *position)
{
    // Check if the position even has a header
    if (!is_managed(position))
    {
        return;
    }
    struct block *blk = get_block_header(position);
    if (get_ref_total(blk) == 0)
    {
        last_unused = free_block(blk, last_unused, is_managed(get_after(blk)));
    }
}

/**
 * Increase the strong users of a block.
 * @param position the position of this block
 */
void increase_strong_users(void *position)
{
    // Check if the position even has a header
    if (!is_managed(position))
    {
        return;
    }
    struct block *blk = get_block_header(position);
    set_ref_strong(blk, get_ref_strong(blk) + 1);
}

/**
 * Increase the total users of a block.
 * @param position the position of this block
 */
void increase_total_users(void *position)
{
    // Check if the position even has a header
    if (!is_managed(position))
    {
        return;
    }
    struct block *blk = get_block_header(position);
    // Increase the block's total references.
    set_ref_total(blk, get_ref_total(blk) + 1);
}

/**
 * Decrease the strong users of a block.
 * @param position the position of this block
 */
int decrease_strong_users(void *position)
{
    // Check if the position even has a header
    if (!is_managed(position))
    {
        return 0;
    }
    struct block *blk = get_block_header(position);
    // Decrease the block's strong references.
    int refs = get_ref_strong(blk) - 1;
    if (refs != 0)
    {
        set_ref_strong(blk, refs);
        return 1;
    }
    else
    {
        return 0;
    }
}

/**
 * Decrease the total users of a block.
 * @param position the position of this block
 */
int decrease_total_users(void *position)
{
    // Check if the position even has a header
    if (!is_managed(position))
    {
        return 0;
    }
    struct block *blk = get_block_header(position);
    // Decrease the block's total references.
    int refs = get_ref_total(blk) - 1;
    if (refs != 0)
    {
        set_ref_total(blk, refs);
        return 1;
    }
    else
    {
        return 0;
    }
}

/**
 * Check if a position is inside of a given block.
 */
int in_block(void *block, void *position)
{
    // Check if the position even has a header
    struct block *blk = get_block_header(block);
    return is_managed(block) && is_managed(position) &&
           (uint8_t *) position >= (uint8_t *) block &&
           (uint8_t *) block + get_size(blk) > (uint8_t *) position;
}
