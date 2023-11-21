#include "galloc.h"
#include "mem.h"
#include "types.h"

// The last unused block by gcat
struct block *last_unused = NULL;

/**
 * Get the next unused block above a certain size.
 * @pre there is at least one unused block in gcat_mem
 * @param size the size of the block to get
 * @return a pointer to the next unused block.
 */
struct block *get_unused(size_t size)
{
    struct block *position;
    for (position = last_unused;
        get_size(position) < size || get_next(position) != last_unused;
        position = get_next(position));
    return position;
}

/**
 * Get a block's header
 */
struct block *get_block_header(void *pointer)
{
    // Check if the pointer even has a header
    if (!is_managed(pointer))
    {
        return NULL;
    }

    // The payload pointer
    size_t *payload = (size_t *) pointer;

    // The block pointer
    return (struct block *) (payload - offsetof(struct block, payload));
}

/**
 * Initialize a struct block.
 * @pre there is not a block at position
 * @post there is now a block at position, with the used bits of prev/next modified
 * @param position the position of the new block
 * @param block_size the size of the new block
 * @param prev the previous block
 * @param next the next block
 */
void make_block(struct block *position, struct block *prev, struct block *next,
    liberty is_unused, size_t block_size, gcat_reaper finalizer)
{
    // Get the block from the pointer
    struct block blk = *position;

    if (prev == NULL)
    {
        prev = position;
    }

    if (next == NULL)
    {
        next = position;
    }

    // Set the previous and next fields of this block to the correct values
    if (is_unused == unused)
    {
        set_prev(position, prev);
        set_next(position, next);
        if (prev)
        {
            // Set up the previous block, as well as the prev_unused value of this one
            struct block prev_block = *prev;
            blk.flags.prev_unused = prev_block.flags.unused;
            prev_block.header.unused_block.pointers.next = position;
        }
        else
        {
            // Set the previous block to be unused
            blk.flags.prev_unused = used;
        }

        if (next)
        {
            // Set up the next block if it exists
            struct block next_block = *next;
            next_block.flags.prev_unused = unused;
            next_block.header.unused_block.pointers.prev = position;
        }
    }
    else
    {
        update_ref_strong(position, 1);
    }

    // Initialize size correctly
    set_size(position, block_size);

    // Set the destructor
    blk.header.used_block.finalizer = finalizer;
}

/**
 * Initialize GCAT's allocator memory if it has not been initialized yet.
 * @post memory initialized
 */
void init_mem()
{
    last_unused = get_mem(NULL);
}
