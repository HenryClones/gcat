#include "blocks.h"
#include "mem.h"
#include "galloc.h"

#define INITIAL_SIZE 1000

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
        set_prevflag(ptr, used);
        set_flag(ptr, used, 0);
        set_size(ptr, INITIAL_SIZE);
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
    last_unused = find_mem(last_unused);
    // walk down the chain and grab the first free block's payload
    struct block *position;
    for (position = get_mem(last_unused);
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
    struct block *blk = get_block_header(block);
    struct block *next = get_next(blk);
    struct block *prev = get_prev(blk);

    set_size(blk, size);
    size_t next_size = block_full_size(blk);
    int has_after = is_managed(get_payload(blk) + next_size);
    set_flag(blk, used, has_after);
    set_finalizer(blk, finalizer);

    if (next_size >= sizeof(struct block) + sizeof(size_t) * 2)
    {
        struct block *after = get_after(blk);
        if (next == blk)
        {
            next = after;
        }
        if (prev == blk)
        {
            prev = after;
        }

        set_size(after, next_size);
        set_flag(after, unused, is_managed(get_after(blk)));
        set_next(after, next);
        set_next(after, prev);
        last_unused = after;
    }

    return get_payload(blk);
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
