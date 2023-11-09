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
    size_t *boundary = get_block_boundary(blk, blk->size);
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