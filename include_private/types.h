#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#ifndef gcat_reaper
typedef void(* gcat_reaper)(void *);
#endif

#ifndef memory_area
// 64-bit aligned memory area
#ifdef __BIGGEST_ALIGNMENT__

#endif
typedef struct __attribute__(__aligned__) aligned_bytes {
    uint8_t bytes[1];
} memory_area;
#endif

#ifndef liberty
// Determine whether a block is free or used
typedef enum {free, used} liberty;
#endif

#ifndef struct block
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
            gcat_reaper finalizer;
        } used_block;
    } header;

    // The payload, offsetof must work here
    // uint64_t forces alignment on 64-bit systems for now
    // Ends with the size
    memory_area payload;
};
#endif

#ifdef __cplusplus
}
#endif // __cplusplus
