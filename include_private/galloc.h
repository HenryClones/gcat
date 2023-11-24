#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#ifndef GCAT_GALLOC_H
#define GCAT_GALLOC_H

#ifndef size_t
#include <stddef.h>
#endif // size_t

void make_block_free(void *position);
void *use_block(void *block, size_t size);
void increase_strong_users(void *position);
void increase_total_users(void *position);
void decrease_strong_users(void *position);
void decrease_total_users(void *position);
int in_block(void *block, void *position);

#endif // GCAT_GALLOC_H

#ifdef __cplusplus
}
#endif // __cplusplus
