#include "blocks.h"

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
 * @param prev the block previous in the unused list
 */
void set_prev(struct block blk, struct block prev)
{
    blk.header.unused_block.pointers.prev = &prev;
}

/**
 * Set the next block pointer.
 * @pre blk is unused
 * @param blk the block to change the position of in the unused list
 * @return the block to be next in the unused list
 */
void set_next(struct block blk, struct block next)
{
    blk.header.unused_block.pointers.next = &next;
}

/**
 * Get the previous block pointer.
 * @pre blk is unused
 * @param blk the block to get the position of in the unused list
 * @return the block previous in the unused list
 */
struct block get_prev(struct block blk)
{
    return *(blk.header.unused_block.pointers.prev);
}

/**
 * Get the next block pointer.
 * @pre blk is unused
 * @param blk the block to get the position of in the unused list
 * @return the block next in the unused list
 */
struct block get_next(struct block blk)
{
    return *(blk.header.unused_block.pointers.next);
}
