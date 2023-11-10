#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#ifndef GCAT_TYPES_H
#define GCAT_TYPES_H

#include <stdint.h>
#include <stddef.h>

typedef void(* gcat_reaper)(void *);

// Determine whether a block is unused or used
typedef enum {unused, used} liberty;

#define VARIABLE_LENGTH_ARRAY 1
// Handle every type of block
struct block
{
    // The size of a block.
    size_t size;
    // Flag data associated with a block
    struct
    {
        // coalescence: unused -> !prev_unused
        liberty unused;
        liberty prev_unused;
    } flags;

    union
    {
        struct
        {
            // The pointers in the explicit unused list.
            struct
            {
                // treat as implementation dependent?
                struct block *prev;
                struct block *next;
            } pointers;
        } unused_block;

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
    } header __attribute__((aligned));

    // The payload, offsetof must work here
    // uint64_t forces alignment on 64-bit systems for now
    // Ends with the size
    uint8_t payload[VARIABLE_LENGTH_ARRAY];
};

#endif // GCAT_TYPES_H

#ifdef __cplusplus
}
#endif // __cplusplus
