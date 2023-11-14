#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#ifndef GCAT_TYPES_H
#define GCAT_TYPES_H

#include <stdint.h>
#include <stddef.h>

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
#endif // NO_UB

typedef void(* gcat_reaper)(void *);

// Determine whether a block is unused or used
typedef enum {unused, used} liberty;

#endif // GCAT_TYPES_H

#ifdef __cplusplus
}
#endif // __cplusplus
