#include "blocks.h"
#include "mem.h"
#include "galloc.h"

// The last unused block by gcat
struct block *last_unused = NULL;

/**
 * Get the next unused block above a certain size.
 * @pre there is at least one unused block in gcat_mem
 * @param size the size of the block to get
 * @return a position to the next unused block.
 */
void *get_unused(size_t size)
{
    struct block *position;
    for (position = get_mem(last_unused);
        get_size(position) < size || get_next(position) != last_unused;
        position = get_next(position));
    return position->payload;
}

/**
 * Get a block's header.
 * @param position the position to the block
 * @return the block's header position, or NULL if it is not in the right area
 */
static struct block * __attribute__ ((const)) get_block_header(void *position)
{
    // The payload position
    uint8_t * payload = (uint8_t *) position;

    // The block position
    return (struct block *) (payload - offsetof(struct block, payload));
}

/**
 * Use a block, splitting extra space off to the right.
 * 
 */
void *use_block(void *block, void (*finalizer)(void *), size_t size)
{
    struct block *blk = get_block_header(block);
    set_flag(blk, used, is_managed(get_after(blk)));
    return blk->payload;
}

/**
 * Free a struct block if necessary by applying recursive cases.
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
    free_block(blk, last_unused, is_managed(get_after(blk)));
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
    update_ref_strong(blk, 1);
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
    update_ref_total(blk, 1);
}

/**
 * Decrease the strong users of a block.
 * @param position the position of this block
 */
void decrease_strong_users(void *position)
{
    // Check if the position even has a header
    if (!is_managed(position))
    {
        return;
    }
    struct block *blk = get_block_header(position);
    // Decrease the block's strong references.
    update_ref_strong(blk, -1);
}

/**
 * Decrease the total users of a block.
 * @param position the position of this block
 */
void decrease_total_users(void *position)
{
    // Check if the position even has a header
    if (!is_managed(position))
    {
        return;
    }
    struct block *blk = get_block_header(position);
    // Decrease the block's total references.
    update_ref_total(blk, -1);
}

/**
 * Check if a position is inside of a given block.
 */
int in_block(void *block, void *position)
{
    // Check if the position even has a header
    struct block *blk = get_block_header(block);
    return is_managed(block) && is_managed(position) &&
           position >= block && blk->size + block > position;
}
