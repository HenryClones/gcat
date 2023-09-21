#include <pthread.h>
#include <wrappers.h>
#include <gcat.h>

/**
 * Header size as an offset.
 */
#define HEADER_SIZE 0x20

/**
 * The initial size of GCAT's garbage collected pages.
 */
#define GCAT_INITIAL_MANAGED_PAGE_SIZE 65536

/**
 * Initial amount of users for a new block.
 */
#define GCAT_INITIAL_USERS 1ULL

/**
 * Bit mask for the users of a block.
 */
#define GCAT_BLOCK_HEADER_MASK_USERS (0x3FFFFFFFULL << 34ULL)

/**
 * Size of a block in bytes.
 */
#define GCAT_BLOCK_SIZE (0xFFFFFFFFULL)

/**
 * Bit mask for size of a block in bytes.
 */
#define GCAT_BLOCK_HEADER_MASK_SIZE (GCAT_BLOCK_SIZE << 2ULL)

/**
 * Bit mask for free bit in the previous block.
 */
#define GCAT_BLOCK_HEADER_MASK_PREV_FREE_BIT 0x2ULL

/**
 * Bit mask for free bit in a block.
 * If this is set, the GCAT_BLOCK_HEADER_MASK_PREV_FREE_BIT must be 0.
 */
#define GCAT_BLOCK_HEADER_MASK_FREE_BIT 0x1ULL

/**
 * The header of a gcat block.
 */
#define GCAT_BLOCK_HEADER(ptr) (((gcat_block *) ptr)->block_header)

/**
 * Get the free bit for a block.
 */
#define GCAT_GETMASK_FREE(ptr) ((int) (GCAT_BLOCK_HEADER(ptr) & GCAT_BLOCK_HEADER_MASK_FREE_BIT))

/**
 * Get the previous block's free bit.
 */
#define GCAT_GETMASK_PREV(ptr) ((int) (GCAT_BLOCK_HEADER(ptr) & GCAT_BLOCK_HEADER_MASK_PREV_FREE_BIT))

/**
 * Get the free bit for a block.
 */
#define GCAT_SETMASK_FREE(ptr, free) GCAT_BLOCK_HEADER(ptr) = GCAT_BLOCK_HEADER(ptr) | (free ? GCAT_BLOCK_HEADER_MASK_FREE_BIT : 0)

/**
 * Get the previous block's free bit.
 */
#define GCAT_SETMASK_PREV(ptr, free) GCAT_BLOCK_HEADER(ptr) = GCAT_BLOCK_HEADER(ptr) | (free ? GCAT_BLOCK_HEADER_MASK_PREV_FREE_BIT : 0)

/**
 * Get the users in the gcat block header.
 */
#define GCAT_GETUSER(ptr) ((GCAT_BLOCK_HEADER(ptr) & GCAT_BLOCK_HEADER_MASK_USERS) >> 34ULL)

/**
 * Get the users in the gcat block header. (Does not assign to it like adduser and removeuser)
 */
#define GCAT_SETUSER(ptr, users) ((GCAT_BLOCK_HEADER(ptr) & ~GCAT_BLOCK_HEADER_MASK_USERS) | (users << 34ULL))

/**
 * Add a user to a GCAT block.
 */
#define GCAT_ADDUSER(ptr) GCAT_BLOCK_HEADER(ptr) |= GCAT_SETUSER(ptr, GCAT_GETUSER(ptr))

/**
 * Remove a user from a GCAT block.
 */
#define GCAT_RMUSER(ptr) GCAT_BLOCK_HEADER(ptr) |= GCAT_SETUSER(ptr, GCAT_GETUSER(ptr))

/**
 * This value indicates that the size of an array may change at runtime.
 */
#define ARRAY_VARIABLE_LENGTH 1

/**
 * Memory block.
 */
typedef struct block
{
    struct block *prev;
    struct block *next;
    unsigned long long int block_header;
    char payload[ARRAY_VARIABLE_LENGTH];
} gcat_block;

/**
 * The allocator's garbage-collector managed memory block.
 */
static gcat_block *gcat_mem = NULL;

/**
 * The size of the above block.
 */
static long long int gcat_size = GCAT_INITIAL_MANAGED_PAGE_SIZE;

/**
 * Initialize a gcat_block.
 * @pre there is not a block at position
 * @post there is now a block at position, with the used bits of prev/next modified
 * @param position the position of the new block
 * @param block_size the size of the new block
 * @param prev the previous block
 * @param next the next block
 * @param free whether the block is free
 */
static void make_gcat_block(void* position, size_t block_size,
    gcat_block *prev, gcat_block *next, unsigned int free)
{
    // Make sure free is the right value
    if (free > 1)
    {
        free = 1;
    }
    // Initialize the block to be a gcat block at position.
    gcat_block b = *((gcat_block *) position);
    b.prev = prev;
    b.next = next;
    // Initialize the "is previous block free" bit of this block
    int prev_bit = 0;
    if (prev != NULL)
    {
        prev_bit = GCAT_GETMASK_FREE(prev) << 1ULL;
        prev->next = (gcat_block*) position;
    }
    if (next != NULL)
    {
        GCAT_SETMASK_FREE(next, free);
        next->prev = (gcat_block*) position;
    }
    b.block_header = free | prev_bit | (block_size << 2ULL) |
        (GCAT_INITIAL_USERS << 34ULL);
}

/**
 * Initialize the block of memory used for dynamic allocation,
 * as well as prerequisites for garbage collection.
 * @pre gcat has no managed memory page
 * @post the garbage collected segment
 */
static void init_gc()
{
    // Initialize gcat_mem
    int fd = Open("/dev/zero");
    gcat_mem = Mmap(NULL, GCAT_MANAGED_PAGE_SIZE, fd);
    Close(fd);
    // Initialize the "dummy" block
    make_gcat_block(gcat_mem, 0, NULL, NULL, 0);
    // Initialize the free blocks to fill the memory
    make_gcat_block(gcat_mem + sizeof(gcat_block),
        GCAT_INITIAL_MANAGED_PAGE_SIZE - 2 * sizeof(gcat_block),
        gcat_mem, gcat_mem, 1);
    GCAT_ADDUSER(gcat_mem);
}

/**
 * Expand gcat's created block.
 * @pre gcat has a managed memory page.
 * @post the garbage collected page is at least as big as it was initially
 */
void *grow(void* block)
{
    return block;
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
    if ((gcat_block *) block < gcat_mem || (gcat_block *) block > (gcat_mem + gcat_size))
    {
        return;
    }
    gcat_block *actual_block = (gcat_block *) block;
    GCAT_RMUSER(actual_block);
}

/**
 * GCAT's customized garbage-collected memory allocator.
 * @post there is a used block with one user which was returned.
 * @return The memory which was allocated, or NULL if it failed.
 */
void *gall(int size)
{
    // Initialize gcat_mem if it does not exist
    if (gcat_mem == NULL)
    {
        init_gc();
    }
    // Go to the first block
    gcat_block *block = gcat_mem;
    // Seek until a free block is found, the free block is b
    gcat_block *b;
    for (b = block; b != NULL && !GCAT_GETMASK_FREE(b); b = b->next);
    // Allocate a block into b
    make_gcat_block(b, size, b->prev, b->next, 0);
    GCAT_ADDUSER(b);
    // Return the block
    return b->payload;
}
