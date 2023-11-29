#include <string.h>
#include <stdint.h>
#include "gcat.h"
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
    if (x != z || *x != *z)
    {
        return 1;
    }
    return 1;
}

/**
 * Test gcat.h gall.
 */
static int gcat_test02()
{
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

/**
 * Test gcat.h burr_stack.
 */
static int gcat_test05()
{
    return 0;
}

/**
 * Test gcat.h burr_heap.
 */
static int gcat_test06()
{
    return 0;
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
