#include <stdint.h>
#include <pthread.h>
#include <wrappers.h>
#include <gcat.h>

/**
 * The initial size of GCAT's garbage collected pages.
 */
#define GCAT_INITIAL_MANAGED_PAGE_SIZE 65536ULL

typedef uint64_t memory_area[1];

// Block type
typedef enum {vector, finalized} block_class;

// Free value
typedef enum {free, used} liberty;

// Handle every type of block
struct block
{
    // The pointers in a block.
    struct
    {
        struct block *prev;
        struct block *next;
    } pointers;
    // A block's users are stored as a unit.
    struct
    {
        uint32_t total_users;
        uint32_t strong_users;
    } users;
    // Flag data associated with a block
    struct
    {
        block_class type;
        liberty free;
        liberty prev_free;
    } flags;
    // The size of a block.
    size_t size;
    // The finalizer, if defined
    reaper finalizer;
    // The payload
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
 * @pre block is a valid block which is currently used
 * @post block has one more weak reference
 * @param block the pointer to the block in GCAT to add a reference to
 */
static void add_ref_total(struct block blk)
{
    ++(blk.users.total_users);
}

/**
 * Add a strong reference to the current block.
 * @pre block is a valid block which is currently used
 * @post block has one more strong reference
 * @param block the pointer to the block in GCAT to add a reference to
 */
static void add_ref_strong(struct block blk)
{
    ++(blk.users.strong_users);
    add_ref_total(blk);
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
    liberty free, size_t block_size, reaper finalizer)
{
    // Get the block from the pointer
    struct block blk = *position;

    // Set the previous and next fields of this block to the correct values
    blk.pointers.prev = prev;
    blk.pointers.next = next;

    if (prev)
    {
        // Set up the previous block, as well as the prev_free value of this one
        struct block prev_block = *prev;
        blk.flags.prev_free = prev_block.flags.free;
        prev_block.pointers.next = position;
    }
    else
    {
        // Set the previous block to be free
        blk.flags.prev_free = used;
    }

    // Set the pointer to the previous block
    blk.pointers.prev = prev;

    if (next)
    {
        // Set up the next block if it exists
        struct block next_block = *next;
        next_block.flags.prev_free = free;
        next_block.pointers.prev = position;
    }

    // Set the pointer to the next block
    blk.pointers.next = next;

    // Set the destructor and block type
    if (finalizer)
    {
        blk.flags.type = finalized;
    }
}

typedef int boolean;

/**
 * Determine if a pointer is to GCAT's managed memory.
 * @param block the pointer to check
 * @return 1 if it is in GCAT's spaced, 0 otherwise
 */
static boolean is_managed(void * block)
{
    return block >= (void *) gcat_mem->payload &&
        block <= (void *) (gcat_mem + gcat_size);
}

/**
 * Get a page of memory for GCAT.
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
static void init_gc()
{
    gcat_mem = get_page();
    // Initialize the "dummy" block
    make_block(gcat_mem, NULL, NULL, used, 0, NULL);
    // Initialize the free blocks to fill the memory
    make_block((void *)gcat_mem->payload, gcat_mem, gcat_mem, free,
        gcat_size - sizeof(*gcat_mem), NULL);
    add_ref_strong(*gcat_mem);
}

/**
 * Remove a user from a gall-ocated block.
 * @pre the block provided is allocated and gcat's garbage collector was initialized.
 * @post the block returned has one less user.
 */
void burr(void *block)
{

}

/**
 * GCAT's customized garbage-collected memory allocator.
 * @post there is a used block with one user which was returned.
 * @return The memory which was allocated, or NULL if it failed.
 */
void *gall(int size, reaper destructor)
{
    // Initialize gcat_mem if it does not exist
    if (gcat_mem == NULL)
    {
        init_gc();
    }
    // Go to the first block
    struct block *block = gcat_mem;
    // Return the block
    return block->payload;
}
