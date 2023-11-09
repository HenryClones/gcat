#include "blocks.h"
#include "wrappers.h"
#include "mem.h"

// Gcat's memory region
struct block *gcat_mem = NULL;
// The size of gcat's memory region
size_t gcat_size = 0;

// The last unused block by gcat
struct block *last_unused = NULL;

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

/**
 * Expand the memory afforded to GCAT.
 */
void expand_mem()
{
    grow_mem(get_newsize(gcat_size));
}

/**
 * Free a block.
 * @pre block is used and has no users and last_unused != NULL
 * @post block will be unusedd up and coalesced
 */
void free_block(struct block blk)
{
    if (blk.header.used_block.finalizer)
    {
        // Execute finalizer over payload
        blk.header.used_block.finalizer(blk.payload);
    }
    // The block is now unusedd
    blk.flags.unused = unused;
    // Be unused
    make_block(&blk, last_unused, last_unused, unused, blk.size, NULL);
    // And reassign the correct one
    last_unused = &blk;
}

/**
 * Get the next unused block above a certain size.
 * @pre there is at least one unused block in gcat_mem
 * @param size the size of the block to get
 * @return a pointer to the next unused block.
 */
struct block *get_unused(size_t size)
{
    struct block *position;
    for (
        position = last_unused;
        position + size < gcat_mem + gcat_size &&
        position != last_unused &&
        position != NULL;
        position = position->header.unused_block.pointers.next
        );
    return position;
}

/**
 * Coalesce all unused blocks around this one.
 * @pre blk is unused
 * @post blk may not be valid and is agglomerated as a larger unused block
 */
void coalesce(struct block *blk)
{
    struct block *next = blk + blk->size;
    // Next block unused coalesce
    if (next->flags.unused == unused)
    {
        blk->header.unused_block.pointers.next = next->header.unused_block.pointers.next;
        blk->size += next->size;
    }
    // Previous block unused coalesce
    if (blk->flags.prev_unused == unused)
    {
        size_t size = *(((size_t *) blk) - 1);
        struct block *prev = blk - size;
        prev->size += blk->size;
        blk->header.unused_block.pointers.prev = prev->header.unused_block.pointers.prev;
        prev->header = blk->header;
        blk = prev;
    }

    // Add a block boundary
    size_t *boundary = get_block_boundary(*blk);
    *boundary = blk->size;
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
    // Initialize the unused blocks to fill the memory
    void *location = (void *)gcat_mem;
    last_unused = location;
    make_block(location, location, location, unused,
        gcat_size - sizeof(struct block), NULL);
}

/**
 * Determine if a pointer is to GCAT's managed memory.
 * @param block the pointer to check
 * @return 1 if it is in GCAT's spaced, 0 otherwise
 */
int is_managed(void *block)
{
    return UB_pointer_gte(block, (void *) (gcat_mem->payload)) &&
        UB_pointer_lte(block, (void *) (gcat_mem + gcat_size));
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
        set_prev(blk, *prev);
        set_next(blk, *next);
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
    set_size(*position, block_size);

    // Set the destructor
    blk.header.used_block.finalizer = finalizer;
}
