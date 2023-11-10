#include <stdint.h>
#include "mem.h"
#include "blocks.h"
#include "galloc.h"
#include "gcat.h"

extern struct block *last_unused;
extern struct block *gcat_mem;
extern size_t gcat_size;

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
 * Remove a user from a gall-ocated block.
 * @pre the block provided is allocated and gcat's garbage collector was initialized.
 * @post the block returned has one less user.
 * @return The memory which was allocated, or NULL if it failed.
 */
void burr(void *block)
{
    // Calculate the offset to the next field
    int off = sizeof(unsigned long long int) + 2 * sizeof(void *);
    block -= off;
    // Change it to an if statement
    struct block *actual_block = (struct block *) block;
    update_ref_strong(*actual_block, -1);
}

/**
 * GCAT's customized garbage-collected memory allocator.
 * @post there is a used block with one user which was returned.
 * @return The memory which was allocated, or NULL if it failed.
 */
void *gall(size_t size, void *finalizer)
{
    // Fit a size_t in the size for the footer
    size = size > sizeof(size_t) ? size : sizeof(size_t);

    // Initialize gcat_mem if it does not exist
    if (gcat_mem == NULL)
    {
        init_mem();
    }
    
    // Find a block
    struct block *position = get_unused(size);
    
    // If finding a block failed
    if (position == unused)
    {
        expand_mem(gcat_mem, &gcat_size);
        return gall(size, finalizer);
    }
    else
    {
        // For testing if it is a full block
        size_t size_padding = position->size - size;
        if (size_padding > sizeof(struct block))
        {
            // Make a unused block
            struct block *new_unused_block = position + size +
                sizeof(struct block);
            make_block(new_unused_block,
                position->header.unused_block.pointers.prev,
                position->header.unused_block.pointers.next,
                used, size_padding, NULL);
            last_unused = new_unused_block;
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
            used, size, (gcat_reaper) finalizer);
        return position->payload;
    }
    
    return NULL;
}
