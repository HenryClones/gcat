#include <pthread.h>
#include <wrappers.h>
#include <gcat.h>

/**
 * The initial size of GCAT's garbage collected pages.
 */
#define GCAT_INITIAL_MANAGED_PAGE_SIZE 65536

/**
 * Initial amount of users for a new block.
 */
#define GCAT_INITIAL_STRONG_USERS 1ULL

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
#define GCAT_BLOCK_SIZE(ptr) (ptr->block_header >> 2ULL)

/**
 * Get the free bit for a block.
 */
#define GCAT_GETMASK_FREE(ptr) ((int) (ptr->block_header & GCAT_BLOCK_HEADER_MASK_FREE_BIT))

/**
 * Get the previous block's free bit.
 */
#define GCAT_GETMASK_PREV(ptr) ((int) (ptr->block_header & GCAT_BLOCK_HEADER_MASK_PREV_FREE_BIT))

/**
 * Set the free bit for a block.
 */
#define GCAT_SETMASK_FREE(ptr, free) ptr->block_header = ptr->block_header | (free ? GCAT_BLOCK_HEADER_MASK_FREE_BIT : 0)

/**
 * Set the previous block's free bit.
 */
#define GCAT_SETMASK_PREV(ptr, free) ptr->block_header = ptr->block_header | (free ? GCAT_BLOCK_HEADER_MASK_PREV_FREE_BIT : 0)

/**
 * This value indicates that the size of an array may change at runtime.
 */
#define ARRAY_VARIABLE_LENGTH 1

/**
 * Free gcat_block (boolean).
 */
#define GCAT_FREE 0x0

/**
 * Used gcat_block (boolean).
 */
#define GCAT_USED 0x1

/**
 * Previous free gcat_block (boolean)
 */
#define GCAT_PREV_FREE 0x2

/**
 * Memory block.
 */
typedef struct block
{
    struct block *prev;
    struct block *next;
    unsigned long long int strong_users;
    unsigned long long int weak_users;
    unsigned long long int block_header;
    char payload[ARRAY_VARIABLE_LENGTH];
} gcat_block;

/**
 * Header size as an offset.
 */
#define GCAT_BLOCK_UNUSABLE (sizeof(struct block *) * 2 + sizeof(unsigned long long int) * 3)

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
static void make_gcat_block(void *position, size_t block_size,
    gcat_block *prev, gcat_block *next, unsigned int free)
{
    // Make sure free is the right value
    if (free != GCAT_USED && free != GCAT_FREE)
    {
        free = GCAT_FREE;
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
        GCAT_SETMASK_PREV(next, free);
        next->prev = (gcat_block*) position;
    }
    b.block_header = free | prev_bit | (block_size << 2ULL);
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
    char *file_page = "/dev/zero";
    int fd = Open(file_page);
    gcat_mem = Mmap(NULL, GCAT_MANAGED_PAGE_SIZE, fd);
    Close(fd, file_page);
    // Initialize the "dummy" block
    make_gcat_block(gcat_mem, 0, NULL, NULL, GCAT_USED);
    // Initialize the free blocks to fill the memory
    make_gcat_block(gcat_mem + sizeof(gcat_block),
        GCAT_INITIAL_MANAGED_PAGE_SIZE - 2 * sizeof(gcat_block),
        gcat_mem, gcat_mem, GCAT_FREE);
    ++gcat_mem->strong_users;
}

static int is_managed(void * block)
{
    return block != NULL && (gcat_block *) block - GCAT_BLOCK_UNUSABLE >= gcat_mem ||
        (gcat_block *) block <= (gcat_mem + gcat_size);
}

/**
 * Expand gcat's created block.
 * @pre gcat has a managed memory page.
 * @post the garbage collected page is at least as big as it was initially
 * @param block the block to expand
 * @return the new block (even if it was recreated) which must be used from now on
 */
void *grow(void *block, int new_size)
{
    // TODO: finish adding guard cases
    gcat_block * b = block - GCAT_BLOCK_UNUSABLE;
    if (!is_managed(block))
    {
        return block;
    }
    int size = GCAT_BLOCK_SIZE(b);
    gcat_block *next_block = block + size + GCAT_BLOCK_UNUSABLE;
    return block;
}

/**
 * Check if an index is in array bounds.
 * @pre gcat has a managed memory page.
 * @post the garbage collected page is at least as big as it was initially
 * @param block the block to expand
 * @return the new block (even if it was recreated) which must be used
 */
void *gcat_array_check(void *block, int index)
{
    gcat_block *b = block - GCAT_BLOCK_UNUSABLE;
    int bounds = GCAT_BLOCK_SIZE(b);
    if (index < 0 || index >= bounds)
    {
        Kill_offset("indexing array", block, index);
    }
    return block + index;
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
    if (!is_managed(block))
    {
        return;
    }
    gcat_block *b = (gcat_block *) block;
    --b->strong_users;
    if (b->strong_users == b->weak_users)
    {
        int size = GCAT_BLOCK_SIZE(b);
        ((int *) b->payload)[size - 1] = size;
        GCAT_SETMASK_FREE(b, GCAT_FREE);
        GCAT_SETMASK_PREV(b->next, GCAT_FREE);
    }
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
    make_gcat_block(b, size, b->prev, b->next, GCAT_USED);
    b->strong_users = GCAT_INITIAL_STRONG_USERS;
    // Return the block
    return b->payload;
}
