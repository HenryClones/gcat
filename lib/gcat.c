#include <stdint.h>
#include "../include_private/wrappers.h"
#include "../include_private/blocks.h"
#include "../include_public/gcat.h"

/**
 * Grab a reference to the pointer for the current function.
 * @pre block points to a valid block within gcat
 * @post block has an additional strong and total reference
 * @param block the gcat block to use
 * @return block
 */
void *hew_function(void *pointer)
{
    // Get the block
    struct block *blk = get_block_header(pointer);
    // Add a strong reference to it
    if (blk)
    {
        update_ref_strong(*blk, 1);
    }
    // Return the pointer regardless
    return pointer;
}

/**
 * Grab a reference to the pointer for an object.
 * @pre block points to a valid block within gcat
 * @post block has an additional total reference
 * @param block the gcat block to use
 * @return block
 */
void *hew_object(void *pointer)
{
    // Get the block
    struct block *blk = get_block_header(pointer);
    // Add a strong reference to it
    if (blk)
    {
        update_ref_total(*blk, 1);
    }
    // Return the pointer regardless
    return pointer;
}

/**
 * Access a payload with bounds checks applied.
 * @param pointer the pointer to a payload
 * @param base the base index in bytes
 * @param offset the offset in bytes
 * @param step the multiplier to offset
 */
void *bounds_checked_access(void *pointer, int base, int offset, int step)
{
    // Get the block header
    struct block *blk = get_block_header(pointer);
    int index = base + offset + step;

    // Only check errors if the part in memory is a block
    if (blk)
    {
        // Get the size of the block
        size_t size = blk->size;

        // If the index is out of bounds, return null
        if (index < 0 || index > size)
        {
            return NULL;
        }
    }

    // Otherwise, always return the pointer and index
    return pointer + index;
}

/**
 * GCAT's customized garbage-collected memory allocator.
 * @post there is a used block with one user which was returned.
 * @return The memory which was allocated, or NULL if it failed.
 */
void *gall(size_t size, gcat_reaper finalizer)
{
    // Initialize gcat_mem if it does not exist
    if (gcat_mem == NULL)
    {
        init_mem();
    }

    // Fit a size_t in the size
    size += 1;
    
    // Find a block
    struct block *position;
    for (
        position = last_free;
        position + size < gcat_mem + gcat_size &&
        position != last_free;
        position = position + position->size
        )
    
    // If finding a block failed
    if (position == free)
    {
        grow_mem(get_newsize(gcat_size));
        return gall(size, finalizer);
    }
    else
    {
        // For testing if it is a full block
        size_t size_padding = position->size - size;
        if (size_padding > sizeof(struct block))
        {
            // Make a free block
            struct block *new_free_block = position + size +
                sizeof(struct block);
            make_block(new_free_block,
                position->header.free_block.pointers.prev,
                position->header.free_block.pointers.next,
                used, size_padding, NULL);
            last_free = new_free_block;
            // Return the block
            return position->payload;
        }
        else
        {
            // Add extra padding to avoid internal fragmentation
            size += size_padding;
        }
        // Make a new block
        make_block(position,
            NULL,
            NULL,
            used, size, finalizer);
        return position->payload;
    }
    
    return NULL;
}
