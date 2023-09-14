#include <pthread.h>
#include <sys/mman.h>
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
 * The memory segment can be read and written to, but not executed.
 * It should be protected as it grows upward in the memory when needed.
 */
#define GCAT_MANAGED_PAGE_PROT (PROT_READ | PROT_WRITE | PROT_GROWSUP)

/**
 * Mapped page settings for the created memory segment.
 */
#define GCAT_MANAGED_PAGE_FLAGS (MAP_SHARED)

/**
 * Initial amount of users for a new block.
 */
#define GCAT_INITIAL_USERS 1

/**
 * Bit mask for the users of a block.
 */
#define GCAT_BLOCK_HEADER_MASK_USERS (0x3FFFFFFFULL << 34)

/**
 * Size of a block in bytes.
 */
#define GCAT_BLOCK_SIZE (0xFFFFFFFFULL)

/**
 * Bit mask for size of a block in bytes.
 */
#define GCAT_BLOCK_HEADER_MASK_SIZE (GCAT_BLOCK_SIZE << 2)

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
 * 
 */
#define GCAT_BLOCK_HEADER(ptr) (((long long int *) ptr) + 2)

/**
 * Get the free bit for a block.
 */
#define GCAT_GETMASK_FREE(ptr) ((int) (*GCAT_BLOCK_HEADER(ptr) & GCAT_BLOCK_HEADER_MASK_FREE_BIT))

/**
 * Get the previous block's free bit.
 */
#define GCAT_GETMASK_PREV(ptr) ((int) (*GCAT_BLOCK_HEADER(ptr) & GCAT_BLOCK_HEADER_MASK_PREV_FREE_BIT))

/**
 * Get the free bit for a block.
 */
#define GCAT_SETMASK_FREE(ptr, free) *ptr = *GCAT_BLOCK_HEADER(ptr) | (free ? GCAT_BLOCK_HEADER_MASK_FREE_BIT : 0)

/**
 * Get the previous block's free bit.
 */
#define GCAT_SETMASK_PREV(ptr, free) *ptr = *GCAT_BLOCK_HEADER(ptr) | (free ? GCAT_BLOCK_HEADER_MASK_PREV_FREE_BIT : 0)

/**
 * This value indicates that the size of an array may change at runtime.
 */
#define ARRAY_VARIABLE_LENGTH 1

/**
 * Memory block.
 */
typedef struct
{
    char *prev;
    char *next;
    long long int block_header;
    char payload[ARRAY_VARIABLE_LENGTH];
} gcat_block;

/**
 * The allocator's garbage-collector managed memory block.
 */
static char *gcat_mem = NULL;

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
    char *prev, char *next, unsigned int free)
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
        prev_bit = GCAT_GETMASK_FREE(prev) << 1;
    }
    if (next != NULL)
    {
        GCAT_SETMASK_FREE(next, free);
    }
    b.block_header = free | prev_bit | (block_size << 2) | GCAT_INITIAL_USERS << 34
    /*
    if (next != NULL && GCAT_GETMASK_FREE(position))
    {
        GCAT_SETMASK_FREE(used);
    }
    */
    /*
    if (prev != NULL && GCAT_GETMASK_PREV(position))
    {
        GCAT_SETMASK_PREV(prev, free);
    }
    */
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
    gcat_mem = Mmap(NULL, GCAT_MANAGED_PAGE_SIZE, GCAT_MANAGED_PAGE_PROT,
        GCAT_MANAGED_PAGE_FLAGS, fd, 0);
    // Initialize the "dummy" block
    make_gcat_block(gcat_mem, 0, NULL, NULL, 1);
    // Initialize the free blocks to fill the memory
    make_gcat_block(gcat_mem, 0, gcat_mem, gcat_mem, 0);
}

/**
 * Expand gcat's created block.
 * @pre gcat has a managed memory page.
 * @post the garbage collected page is at least as big as it was initially.
 */
char *greall()
{
    return "";
}

/**
 * Remove a user from a gall-ocated block.
 * @pre the block provided is allocated and gcat's garbage collector was initialized.
 * @post the block returned has one less user.
 * @return The memory which was allocated, or NULL if it failed.
 */
void burr(char *block)
{
    if (block < gcat_mem || block > gcat_mem + gcat_size)
    {
        return block;
    }
    return block;
}

/**
 * GCAT's customized garbage-collected memory allocator.
 * @post there is a used block with one user which was returned.
 * @return The memory which was allocated, or NULL if it failed.
 */
char *gall()
{
    // Initialize gcat_mem if it does not exist
    if (gcat_mem == NULL)
    {
        init_gc();
    }
    return gcat_mem + HEADER_SIZE;
}
