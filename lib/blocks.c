#include "types.h"

/**
 * Add a weak reference to the current block.
 * Implementation dependent.
 * @pre blk is a valid block which is currently used
 * @post blk has one more weak reference
 * @param blk the pointer to the block in GCAT to add a reference to
 */
inline void update_ref_total(struct block blk, int delta)
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
inline void update_ref_strong(struct block blk, int delta)
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
inline int compare_refs(struct block blk)
{
    return blk.header.used_block.users.strong_users -
        blk.header.used_block.users.total_users;
}

/**
 * Set the previous block pointer.
 * @pre blk is free
 * @param blk the block to change the position of in the free list
 */
inline void set_pointer_prev(struct block blk, struct block * prev)
{
    blk.header.free_block.pointers.prev = prev;
}

/**
 * Set the next block pointer.
 * @pre blk is free
 * @param blk the block to change the position of in the free list
 */
inline void set_pointer_next(struct block blk, struct block * next)
{
    blk.header.free_block.pointers.next = next;
}

/**
 * Block boundary.
 * @pure
 * @param blk the block
 * @param size the size for where the boundary is
 * @return the size_t area to place it at
 */
inline size_t *get_block_boundary(void *blk, size_t size)
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
    liberty is_free, size_t block_size, gcat_reaper finalizer)
{
    // Get the block from the pointer
    struct block blk = *position;

    // Set the previous and next fields of this block to the correct values
    if (is_free == free)
    {
        set_pointer_prev(blk, prev);
        set_pointer_next(blk, next);
        if (prev)
        {
            // Set up the previous block, as well as the prev_free value of this one
            struct block prev_block = *prev;
            blk.flags.prev_free = prev_block.flags.free;
            prev_block.header.free_block.pointers.next = position;
        }
        else
        {
            // Set the previous block to be free
            blk.flags.prev_free = used;
        }

        if (next)
        {
            // Set up the next block if it exists
            struct block next_block = *next;
            next_block.flags.prev_free = free;
            next_block.header.free_block.pointers.prev = position;
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
 * Coalesce all free blocks around this one.
 * @pre blk is free
 * @post blk may not be valid and is agglomerated as a larger free block
 */
void coalesce(struct block *blk)
{
    struct block *next = blk + blk->size;
    // Next block free coalesce
    if (next->flags.free == free)
    {
        blk->header.free_block.pointers.next = next->header.free_block.pointers.next;
        blk->size += next->size;
    }
    // Previous block free coalesce
    if (blk->flags.prev_free == free)
    {
        size_t size = *(((size_t *) blk) - 1);
        struct block *prev = blk - size;
        prev->size += blk->size;
        blk->header.free_block.pointers.prev = prev->header.free_block.pointers.prev;
        prev->header = blk->header;
        blk = prev;
    }

    // Add a block boundary
    size_t *boundary = get_block_boundary(blk, blk->size);
    *boundary = blk->size;
}

/**
 * Free a block.
 * @pre block is used and has no users and last_free != NULL
 * @post block will be freed up and coalesced
 */
void free_block(struct block blk)
{
    if (blk.header.used_block.finalizer)
    {
        // Execute finalizer over payload
        blk.header.used_block.finalizer(blk.payload);
    }
    // The block is now freed
    blk.flags.free = free;
    // Be free
    make_block(&blk, last_free, last_free, free, blk.size, NULL);
    // And reassign the correct one
    last_free = &blk;
}

/**
 * Calculate the size to expand GCAT's memory to, in order to ensure capacity.
 * @pure
 * @param size the current size of a memory block.
 * @return the new size
 */
inline size_t get_newsize(size_t size)
{
    return size * 2;
}

/**
 * Grow GCAT's memory.
 * @param newsize the new size of GCAT'S memory
 */
void grow_mem(size_t newsize)
{
    Mremap(gcat_mem, gcat_size, newsize);
}

/**
 * Get a page of memory for GCAT.
 * @return the new page of memory
 */
void get_page()
{
    gcat_mem = Mmap(NULL, GCAT_MANAGED_PAGE_SIZE);
}

/**
 * Initialize the block of memory used for dynamic allocation,
 * as well as prerequisites for garbage collection.
 * @pre gcat has no managed memory page
 * @post gcat_mem is now the managed page
 */
void init_mem()
{
    // Call the above get_page function to set up gcat.
    get_page();
    // Initialize the free blocks to fill the memory
    void *location = (void *)gcat_mem;
    last_free = location;
    make_block(location, location, location, free,
        gcat_size - sizeof(struct block), NULL);
}

/**
 * Determine if a pointer is to GCAT's managed memory.
 * @param block the pointer to check
 * @return 1 if it is in GCAT's spaced, 0 otherwise
 */
inline int is_managed(void *block)
{
    return UB_pointer_gte(block, (void *) (gcat_mem->payload)) &&
        UB_pointer_lte(block, (void *) (gcat_mem + gcat_size));
}

/**
 * Get a block's header
 */
inline struct block *get_block_header(void *pointer)
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
