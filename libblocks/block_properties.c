#include <blocks.h>
#ifdef __BIGGEST_ALIGNMENT__
#define BLOCK_ALIGN __BIGGEST_ALIGNMENT__
#else
#define BLOCK_ALIGN alignof(max_align_t)
#endif // BLOCK_ALIGN

/**
 * Set this block's used flag.
 * @param blk this block
 * @param new the new status of used/free
 */
void set_used(struct block *blk, int new, int has_next)
{
    if (new)
    {
        blk->flags &= ~free;
    }
    else
    {
        blk->flags |= free;
    }

    if (has_next)
    {
        struct block *next = get_after(blk);
        set_prevused(next, new);
    }
}

void init_flags(struct block *blk)
{
    blk->flags = 0;
}

/**
 * Set this block's prev used flag.
 * @param blk this block
 * @param new the new status of used/free
 */
void set_prevused(struct block *blk, int new)
{
    if (new)
    {
        blk->flags &= ~prev_free;
    }
    else
    {
        blk->flags |= prev_free;
    }
}

/**
 * Get this block's used flag.
 * @param blk this block
 * @return the status of used/free
 */
int get_used(struct block *blk)
{
    return !(blk->flags & free);
}

/**
 * Get this block's used flag.
 * @param blk this block
 * @return the status of used/free
 */
int get_prevused(struct block *blk)
{
    return !(blk->flags & prev_free);
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
 * Block boundary.
 * @param blk the block
 * @return the size_t area to place it at
 */
size_t *get_block_boundary(struct block *blk)
{
    size_t *payload = get_payload(blk);
    return payload + get_size(blk) / sizeof(size_t) - 1;
}

/**
 * Set a block's size.
 * @param blk the block to set the size of, it will be at least size
 */
void set_size(struct block *blk, size_t size)
{
    if (size % BLOCK_ALIGN != 0)
    {
        size += BLOCK_ALIGN - size % BLOCK_ALIGN;
    }
    blk->size = size;
    
    // Bottom of block has boundary tag if free
    if (!get_used(blk))
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
uint32_t get_ref_strong(struct block *blk)
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
void set_ref_strong(struct block *blk, uint32_t x)
{
    blk->header.used_block.users.strong_users = x;
}

/**
 * Get the total references of a block.
 * @pre blk is used
 * @return the total users
 */
uint32_t get_ref_total(struct block *blk)
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
void set_ref_total(struct block *blk, uint32_t x)
{
    blk->header.used_block.users.total_users = x;
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
    if (finalizer)
    {
        blk->flags |= has_finalizer;
    }
    else
    {
        blk->flags &= ~has_finalizer;
    }
    blk->header.used_block.finalizer = finalizer;
}

/**
 * Get blk's finalizer.
 * @pre blk is a valid block which is currently used
 * @param blk the pointer to the block in GCAT to add a reference to
 */
void *get_finalizer(struct block *blk)
{
    if (blk->flags & has_finalizer)
    {
        return blk->header.used_block.finalizer;
    }
    return NULL;
}

/**
 * Get a block's header.
 * @param position the position to the block
 * @return the block's header position, or NULL if it is not in the right area
 */
struct block * __attribute__ ((const)) get_block_header(void *position)
{
    // The block position
    return (struct block *) ((uint8_t *) position - offsetof(struct block, payload));
}
