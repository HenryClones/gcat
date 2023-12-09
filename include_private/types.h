#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#ifndef GCAT_TYPES_H
#define GCAT_TYPES_H
#include <stddef.h>

// Determine whether a block is unused or used
typedef enum {
    free = 1 << 0,
    prev_free = 1 << 1,
    has_finalizer = 1 << 2,
    locked = 1 << 3
} block_flags;

#endif // GCAT_TYPES_H

#ifdef __cplusplus
}
#endif // __cplusplus
