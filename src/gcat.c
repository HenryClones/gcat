#include <stdint.h>
#include <pthread.h>
#include <wrappers.h>
#include <gcat.h>

/**
 * The initial size of GCAT's garbage collected pages.
 */
#define GCAT_INITIAL_MANAGED_PAGE_SIZE 65536ULL

typedef uint64_t memory_area[1];

// Free value
typedef enum {free, used} liberty;

// Handle every type of block
struct block
{
    // The size of a block.
    size_t size;
    // Flag data associated with a block
    struct
    {
        // coalescence: free -> !prev_free
        liberty free;
        liberty prev_free;
    } flags;

    union
    {
        struct
        {
            // The pointers in the explicit free list.
            struct
            {
                // treat as implementation dependent?
                struct block *prev;
                struct block *next;
            } pointers;
        } free_block;

        struct
        {
            // A block's users are stored as a unit.
            struct
            {
                // treat as implementation dependent?
                uint32_t total_users;
                uint32_t strong_users;
            } users;
            // The finalizer, if defined
            reaper finalizer;
        } used_block;
    } header;

    // The payload, offsetof must work here
    // uint64_t forces alignment on 64-bit systems for now
    // Ends with the size
    memory_area payload;
};

/**
 * The allocator's garbage-collector managed memory block.
 */
static struct block *gcat_mem = NULL;

/**
 * The size of the above block.
 */
static uint64_t gcat_size = GCAT_INITIAL_MANAGED_PAGE_SIZE;

/**
 * Add a weak reference to the current block.
 * Implementation dependent.
 * @pre blk is a valid block which is currently used
 * @post blk has one more weak reference
 * @param blk the pointer to the block in GCAT to add a reference to
 */
inline static void update_ref_total(struct block blk, int delta)
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
inline static void update_ref_strong(struct block blk, int delta)
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
inline static int compare_refs(struct block blk)
{
    return blk.header.used_block.users.strong_users -
        blk.header.used_block.users.total_users;
}

/**
 * Set the previous block pointer.
 * @pre blk is free
 * @param blk the block to change the position of in the free list
 */
inline static void set_pointer_prev(struct block blk, struct block * prev)
{
    blk.header.free_block.pointers.prev = prev;
}

/**
 * Set the next block pointer.
 * @pre blk is free
 * @param blk the block to change the position of in the free list
 */
inline static void set_pointer_next(struct block blk, struct block * next)
{
    blk.header.free_block.pointers.next = next;
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
static void make_block(struct block *position, struct block *prev, struct block *next,
    liberty is_free, size_t block_size, reaper finalizer)
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

    // Initialize size correctly
    blk.size = block_size;
    ((size_t *) blk.payload)[blk.size - 1] = blk.size;

    // Set the destructor
    blk.header.used_block.finalizer = finalizer;
}

/**
 * Coalesce all free blocks around this one.
 * @pre blk is free
 * @post blk may not be valid and is agglomerated as a larger free block
 */
static void coalesce(struct block *blk)
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
    ((size_t *) blk->payload)[blk->size - 1] = blk->size;
}

/**
 * Free a block.
 * @pre block is used and has no users
 * @post block will be freed up and coalesced
 */
static void free_block(struct block blk)
{
    // Execute finalizer over payload
    blk.header.used_block.finalizer(blk.payload);
    // The block is now freed
    blk.flags.free = free;
    // Be free
    blk.header.free_block.pointers.next = NULL;
    blk.header.free_block.pointers.prev = NULL;
}

/**
 * Get a page of memory for GCAT.
 * @return the new page of memory
 */
static void *get_page()
{
    // Initialize gcat_mem
    char *file_page = "/dev/zero";
    int fd = Open(file_page);
    gcat_mem = Mmap(NULL, GCAT_MANAGED_PAGE_SIZE, fd);
    Close(fd, file_page);
}

/**
 * Initialize the block of memory used for dynamic allocation,
 * as well as prerequisites for garbage collection.
 * @pre gcat has no managed memory page
 * @post gcat_mem is now the managed page
 */
static void init_mem()
{
    // Call the above get_page function to set up gcat.
    get_page();
    // Initialize the "dummy" block
    make_block(gcat_mem, NULL, NULL, used, 0, NULL);
    // Initialize the free blocks to fill the memory
    void *location = (void *)gcat_mem->payload;
    make_block(location, location, location, free,
        gcat_size - sizeof(*gcat_mem), NULL);
    update_ref_strong(*gcat_mem, 1);
}

/**
 * Determine if a pointer is to GCAT's managed memory.
 * @param block the pointer to check
 * @return 1 if it is in GCAT's spaced, 0 otherwise
 */
inline static int is_managed(void *block)
{
    return block >= (void *) gcat_mem->payload &&
        block <= (void *) (gcat_mem + gcat_size);
}

/**
 * Get a block's header
 */
inline static struct block *get_block_header(void *pointer)
{
    // Check if the pointer even has a header
    if (!is_managed(pointer))
    {
        return NULL;
    }

    // The payload pointer
    uint64_t *payload = (uint64_t *) pointer;

    // The block pointer
    return (struct block *) (payload - offsetof(struct block, payload));
}

/**
 * Remove a strong user from a gall-ocated block.
 * @param block the reference to remove
 */
void burr_function(void *pointer)
{
    // Get the block header
    struct block *blk = get_block_header(pointer);
    // Check if the header is valid
    if (blk)
    {
        // Now, if pointer is a valid block, it has been updated
        update_ref_strong(*blk, -1);
        // And if the reference count reaches a threshold, it is freed
        if (compare_refs(*blk) <= 0)
        {
            free_block(*blk);
            coalesce(blk);
        }
    }
}

/**
 * Remove a total user from a gall-ocated block.
 * @param block the reference to remove
 */
void burr_object(void *pointer)
{
    // Get the block header
    struct block *blk = get_block_header(pointer);
    // Check if the header is valid
    if (blk)
    {
        // Now, if pointer is a valid block, it has been updated
        update_ref_total(*blk, -1);
        // And if the reference count reaches a threshold, it is freed
        if (compare_refs(*blk) <= 0)
        {
            free_block(*blk);
        }
    }
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
 * GCAT's customized garbage-collected memory allocator.
 * @post there is a used block with one user which was returned.
 * @return The memory which was allocated, or NULL if it failed.
 */
void *gall(size_t size, reaper destructor)
{
    // Initialize gcat_mem if it does not exist
    if (gcat_mem == NULL)
    {
        init_mem();
    }
    // Go to the first block
    struct block *blk = gcat_mem;
    // Fit a size_t in the size
    size += 1;
    
    // Find a block

    // Return the block
    return blk->payload;
}
