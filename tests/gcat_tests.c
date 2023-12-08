#include <string.h>
#include <stdint.h>
#include <gcat.h>
#include "gcat_tests.h"

/**
 * Test gcat.h bounds_checked_access.
 */
static int gcat_test01()
{
    uint8_t *data1 = gall(40, NULL);
    uint32_t *w = (uint32_t *) bounds_checked_access(data1, 0, 1, 64);
    if (w != NULL)
    {
        return 1;
    }
    uint32_t *x = (uint32_t *) bounds_checked_access(data1, 0, 0, sizeof(uint32_t));
    *x = 30;
    uint8_t *data2 = gall(40, NULL);
    uint32_t *y = (uint32_t *) bounds_checked_access(data2, 0, 1, sizeof(uint32_t));
    *y = 24;
    if (x == y || *x == *y)
    {
        return 1;
    }
    uint32_t *z = (uint32_t *) bounds_checked_access(data1, 0, 1, sizeof(uint32_t));
    *z = 11;
    if (x == z || *x == *z || x == y || *x == *y)
    {
        return 1;
    }
    return 0;
}

/**
 * Test gcat.h gall.
 */
static int gcat_test02()
{
    uint64_t *data1 = gall(32, NULL);
    uint64_t *data2 = gall(32, NULL);
    if (data1 == data2)
    {
        return 1;
    }
    *data1 = 80;
    *data2 = 81;
    if (*data1 == *data2 || *data1 != *data2 - 1)
    {
        return 1;
    }
    return 0;
}

/**
 * Test gcat.h hew_stack.
 */
static int gcat_test03()
{
    return 0;
}

/**
 * Test gcat.h hew_heap.
 */
static int gcat_test04()
{
    return 0;
}

int finalizer_ran = 0;
uint64_t set_value = 200;
/**
 * This function is a finalizer function meant for testing.
 */
static void finalizer(void *payload)
{
    finalizer_ran = ((uint64_t *)payload)[0] == set_value;
}

/**
 * Test gcat.h burr_stack.
 */
static int gcat_test05()
{
    // Special case where =malloc/free
    uint8_t *data[4];
    data[0] = gall(32, NULL);
    burr_stack(data[0]);
    data[1] = gall(32, NULL);
    if (data[0] != data[1])
    {
        return 1;
    }
    data[2] = gall(32, finalizer);
    if (data[0] == data[2])
    {
        return 1;
    }
    if (finalizer_ran == 1)
    {
        return 1;
    }
    data[2][0] = 0;
    burr_stack(data[2]);
    if (finalizer_ran == 1)
    {
        return 1;
    }
    data[3] = gall(64, finalizer);
    data[3][0] = set_value;
    if (data[0] == data[3])
    {
        return 1;
    }
    if (finalizer_ran == 1)
    {
        return 1;
    }
    burr_stack(data[3]);
    if (finalizer_ran == 0)
    {
        return 1;
    }
    return 0;
}

int blocks = 5;
int freed_tally = 0;
static void true_finalizer(void *payload)
{
    // Assume it's NULL-pointer terminated
    void **ptrs = payload;
    void *current = NULL;
    int i;
    for (i = 0, current = ptrs[i]; current != NULL; current = ptrs[i])
    {
        burr_heap(current);
    }
    ++freed_tally;
}

/**
 * Test gcat.h burr_heap.
 */
static int gcat_test06()
{
    void **data[blocks];
    // Triple pointers are hopefully going to solve all of my problems
    int i;
    for (i = 0; i < blocks; ++i)
    {
        // Fit the terminating null pointer
        data[i] = gall(8 * (i + 2), true_finalizer);
    }
    // Do the setting as a separate step
    for (i = 0; i < blocks; ++i)
    {
        int j;
        for (j = 0; j < i; ++j)
        {
            if (i == 0)
            {
                data[i][j] = data[blocks - 1];
            }
            else
            {
                data[i][j] = data[i - 1];
            }
        }
    }
    // Build a really overcomplicated resource graph to challenge fate or w/e
    for (i = 0; i < blocks; ++i)
    {
        if (freed_tally != 0)
        {
            return 1;
        }
        hew_stack(data[i]);
    }
    return freed_tally == blocks && data[0] == gall(64, NULL);
}

/**
 * Test gcat.h.
 */
int gcat_tests(char *test)
{
    int results = 0;
    if (!strcmp(test, "gcat") || !strcmp(test, "gcat01"))
    {
        results |= gcat_test01();
    }

    if (!strcmp(test, "gcat") || !strcmp(test, "gcat02"))
    {
        results |= gcat_test02();
    }

    if (!strcmp(test, "gcat") || !strcmp(test, "gcat03"))
    {
        results |= gcat_test03();
    }

    if (!strcmp(test, "gcat") || !strcmp(test, "gcat04"))
    {
        results |= gcat_test04();
    }

    if (!strcmp(test, "gcat") || !strcmp(test, "gcat05"))
    {
        results |= gcat_test05();
    }

    if (!strcmp(test, "gcat") || !strcmp(test, "gcat06"))
    {
        results |= gcat_test06();
    }

    return results;
}
