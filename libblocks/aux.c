#include "blocks.h"

/**
 * Initialize the fields in a disjoint set.
 */
void make_disj_set(struct disj_set* set, struct block* blk) {
    set->parent = set;
    set->value = blk;
    set->rank = 1;
}

/**
 * Find a disjoint set pointer's ancestor set.
 * @param start the node to find the ancestor set of
 * @return the ancestor set
 */
struct disj_set* find_set(struct disj_set* start)
{
    if (start != start->parent) {
        start->parent = find_set(start);
    }

    return start;
}

/**
 * Make a union of two disjoint sets.
 * @param a Non-null! The left node
 * @param b Non-null! The right node
 * @return the disjoint union of a and b
 */
struct disj_set* make_union(struct disj_set* a, struct disj_set* b)
{
    if (a == b) {
        return a;
    }

    int newRank = a->rank + b->rank;

    if (a->rank >= b->rank) {
        b->parent = a;
        a->rank = newRank;
        return a;
    } else {
        a->parent = b;
        b->rank = newRank;
        return b;
    }
}
