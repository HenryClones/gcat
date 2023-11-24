#include <blocks.h>

/**
 * Set this block's used flag.
 * @param blk this block
 * @param new the new status of used/free
 */
void set_flag(struct block *blk, liberty new, int has_next)
{
    blk->flags.unused = new;

    if (has_next)
    {
        struct block *next = get_after(blk);
        next->flags.prev_unused = new;
    }
}

/**
 * Get this block's used flag.
 * @param blk this block
 * @return the status of used/free
 */
liberty get_flag(struct block *blk)
{
    return blk->flags.unused;
}

/**
 * Get this block's used flag.
 * @param blk this block
 * @return the status of used/free
 */
liberty get_prevflag(struct block *blk)
{
    return blk->flags.prev_unused;
}

/**
 * Get a block's size.
 * @param blk the block to get the size of
 * @return the size of the block
 */
size_t get_size(struct block *blk)
{
    return blk->size;
}

/**
 * Set a block's size.
 * @param blk the block to set the size of, it will be at least size
 */
void set_size(struct block *blk, size_t size)
{
    // Should be alignof?
    #ifndef alignof
    #define alignof sizeof
    #endif
    #define padof(type, x) alignof(type) - x % alignof(type)
    size += padof(*blk, size);
    blk->size = size;
    
    // Bottom of block has boundary tag if free
    if (get_flag(blk) == unused)
    {
        size_t *boundary = get_block_boundary(blk);
        *boundary = size;
    }
}

/**
 * Set the previous block pointer.
 * @pre blk is unused
 * @param blk the block to change the position of in the unused list
 * @param prev the block previous in the unused list
 */
void set_prev(struct block *blk, struct block *prev)
{
    blk->header.unused_block.pointers.prev = prev;
}

/**
 * Set the next block pointer.
 * @pre blk is unused
 * @param blk the block to change the position of in the unused list
 * @return the block to be next in the unused list
 */
void set_next(struct block *blk, struct block *next)
{
    blk->header.unused_block.pointers.next = next;
}

/**
 * Get the previous block pointer.
 * @pre blk is unused
 * @param blk the block to get the position of in the unused list
 * @return the block previous in the unused list
 */
struct block *get_prev(struct block *blk)
{
    return blk->header.unused_block.pointers.prev;
}

/**
 * Get the next block pointer.
 * @pre blk is unused
 * @param blk the block to get the position of in the unused list
 * @return the block next in the unused list
 */
struct block *get_next(struct block *blk)
{
    return blk->header.unused_block.pointers.next;
}

/**
 * Get the strong references of a block.
 * @pre blk is used
 * @return the strong users
 */
int get_ref_strong(struct block *blk)
{
    return blk->header.used_block.users.strong_users;
}

/**
 * Add a strong reference to the current block.
 * Implementation dependent.
 * @pre blk is a valid block which is currently used
 * @post block has one more strong reference
 * @param blk the pointer to the block in GCAT to add a reference to
 */
void update_ref_strong(struct block *blk, int delta)
{
    blk->header.used_block.users.strong_users += delta;
    update_ref_total(blk, delta);
}

/**
 * Get the total references of a block.
 * @pre blk is used
 * @return the total users
 */
int get_ref_total(struct block *blk)
{
    return blk->header.used_block.users.total_users;
}

/**
 * Add a weak reference to the current block.
 * Implementation dependent.
 * @pre blk is a valid block which is currently used
 * @post blk has one more weak reference
 * @param blk the pointer to the block in GCAT to add a reference to
 */
void update_ref_total(struct block *blk, int delta)
{
    blk->header.used_block.users.total_users += delta;
}

/**
 * Get the payload of this block.
 * @param blk the block
 * @return this block's payload
 */
void *get_payload(struct block *blk)
{
    return blk->payload;
}

/**
 * Set blk's finalizer.
 * @pre blk is a valid block which is currently used
 * @param blk the pointer to the block in GCAT to add a reference to
 */
void set_finalizer(struct block *blk, void(* finalizer)(void *))
{
    blk->header.used_block.finalizer = finalizer;
}

/**
 * Get blk's finalizer.
 * @pre blk is a valid block which is currently used
 * @param blk the pointer to the block in GCAT to add a reference to
 */
void* get_finalizer(struct block *blk)
{
    return blk->header.used_block.finalizer;
}
