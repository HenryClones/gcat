#include "blocks.h"

/**
 * Add a weak reference to the current block.
 * Implementation dependent.
 * @pre blk is a valid block which is currently used
 * @post blk has one more weak reference
 * @param blk the pointer to the block in GCAT to add a reference to
 */
void update_ref_total(struct block blk, int delta)
{
    blk.header.used_block.users.total_users += delta;
}

/**
 * Add a strong reference to the current block.
 * Implementation dependent.
 * @pre blk is a valid block which is currently used
 * @post block has one more strong reference
 * @param blk the pointer to the block in GCAT to add a reference to
 */
void update_ref_strong(struct block blk, int delta)
{
    blk.header.used_block.users.strong_users += delta;
    update_ref_total(blk, delta);
}

/**
 * Compare the references of a block.
 * @pre blk is used
 * @param blk
 * @return the difference between strong and total users
 */
int compare_refs(struct block blk)
{
    return blk.header.used_block.users.strong_users -
        blk.header.used_block.users.total_users;
}

/**
 * Set the previous block pointer.
 * @pre blk is unused
 * @param blk the block to change the position of in the unused list
 */
void set_pointer_prev(struct block blk, struct block * prev)
{
    blk.header.unused_block.pointers.prev = prev;
}

/**
 * Set the next block pointer.
 * @pre blk is unused
 * @param blk the block to change the position of in the unused list
 */
void set_pointer_next(struct block blk, struct block * next)
{
    blk.header.unused_block.pointers.next = next;
}

/**
 * Block boundary.
 * @pure
 * @param blk the block
 * @param size the size for where the boundary is
 * @return the size_t area to place it at
 */
size_t *get_block_boundary(void *blk, size_t size)
{
    return ((size_t *) blk) + size / sizeof(size_t) - 1;
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

    // Set the previous and next fields of this block to the correct values
    if (is_unused == unused)
    {
        set_pointer_prev(blk, prev);
        set_pointer_next(blk, next);
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
        update_ref_strong(blk, 1);
    }

    // Initialize size correctly
    blk.size = block_size;
    size_t *boundary = get_block_boundary(position, block_size);
    *boundary = blk.size;

    // Set the destructor
    blk.header.used_block.finalizer = finalizer;
}

/**
 * Calculate the size to expand GCAT's memory to, in order to ensure capacity.
 * @pure
 * @param size the current size of a memory block.
 * @return the new size
 */
size_t get_newsize(size_t size)
{
    return size * 2;
}
