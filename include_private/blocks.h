#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef void(* gcat_reaper)(void *);

/**
 * The initial size of GCAT's garbage collected pages.
 */
#define GCAT_INITIAL_MANAGED_PAGE_SIZE ((size_t) 65536)

#ifndef NO_UB
/**
 * This can compare two pointers for being greater than or equal to each other.
 * This is undefined behavior, and only works for pointers within register size,
 * as well as only working on flat architectures.
 */
#define UB_pointer_gte(ptr1, ptr2) (((uintptr_t) ptr1) >= ((uintptr_t) ptr2))

/**
 * This can compare two pointers for being less than or equal to each other.
 * This is undefined behavior, and only works for pointers within register size,
 * as well as only working on flat architectures.
 */
#define UB_pointer_lte(ptr1, ptr2) (((uintptr_t) ptr1) <= ((uintptr_t) ptr2))
#endif

/**
 * The allocator's garbage-collector managed memory block.
 */
static struct block *gcat_mem = NULL;

/**
 * The allocator's last freed block.
 */
static struct block *last_free = NULL;

inline void update_ref_total(struct block blk, int delta);
inline void update_ref_strong(struct block blk, int delta);
inline int compare_refs(struct block blk);
inline void set_pointer_prev(struct block blk, struct block * prev);
inline void set_pointer_next(struct block blk, struct block * next);
inline size_t *get_block_boundary(void *blk, size_t size);
void make_block(struct block *position, struct block *prev, struct block *next,
    liberty is_free, size_t block_size, gcat_reaper finalizer);
void coalesce(struct block *blk);
void free_block(struct block blk);
inline size_t get_newsize(size_t size);
void grow_mem(size_t newsize);
void get_page();
void init_mem();
inline int is_managed(void *block);
inline struct block *get_block_header(void *pointer);

#ifdef __cplusplus
}
#endif // __cplusplus
