#include <stdint.h>
#include "galloc.h"
#include "gcat.h"

/**
 * Access a payload with bounds checks applied.
 * @param pointer the pointer to a payload
 * @param base the base index in bytes
 * @param offset the offset in bytes
 * @param step the multiplier to offset
 * @return the desired position or NULL if invalid
 */
void *bounds_checked_access(void *pointer, size_t base, size_t offset, size_t step)
{
    void *desired_position = (uint8_t *) pointer + base + offset * step;
    if (in_block(pointer, desired_position))
    {
        return desired_position;
    }

    return NULL;
}

/**
 * Grab a reference to the pointer for the current function.
 * @pre block points to a valid block within gcat
 * @post block has an additional strong and total reference
 * @param block the gcat block to use
 * @return block
 */
void *hew_stack(void *pointer)
{
    increase_strong_users(pointer);
    return pointer;
}

/**
 * Grab a reference to the pointer for an object.
 * @pre block points to a valid block within gcat
 * @post block has an additional total reference
 * @param block the gcat block to use
 * @return block
 */
void *hew_heap(void *pointer)
{
    // Increase the block's total references.
    increase_total_users(pointer);
    return pointer;
}

/**
 * Remove a user from a gall-ocated block.
 * @pre the block provided is allocated and gcat's garbage collector was initialized.
 * @post the block returned has one less user.
 * @return The memory which was allocated, or NULL if it failed.
 */
void burr_stack(void *block)
{
    decrease_strong_users(block);
    make_block_free(block);
}

/**
 * Remove a user from a gall-ocated block.
 * @pre the block provided is allocated and gcat's garbage collector was initialized.
 * @post the block returned has one less user.
 * @return The memory which was allocated, or NULL if it failed.
 */
void burr_heap(void *block)
{
    decrease_total_users(block);
    make_block_free(block);
}

/**
 * GCAT's customized managed memory allocator.
 * @post there is a used block with one user which was returned.
 * @return The memory which was allocated, or NULL if it failed.
 */
void *gall(size_t size, void(* finalizer)(void *))
{
    return use_block(get_unused(size), finalizer, size);
}
