#include "galloc.h"
#include "galloc_tests.h"
#include "blocks.h"
#include "string.h"

/**
 * Test galloc.h get_unused.
 */
int galloc_test01()
{
    struct block *x = get_unused(10);
    struct block *y = get_unused(10);
    if (x != y)
    {
        return 1;
    }
    return 0;
}

/**
 * Test galloc.h use_block.
 */
int galloc_test02()
{
    struct block *x = get_block_header(get_unused(64));
    struct block *z = get_block_header(use_block(get_payload(x), NULL, 10));
    struct block *y = get_block_header(get_unused(10));
    if (get_after(z) != y || x == y || x != z)
    {
        return 1;
    }
    return 0;
}

/**
 * Test galloc.h make_block_free.
 */
int galloc_test03()
{
    void *x = get_unused(64);
    void *z = use_block(x, NULL, 64);
    set_ref_total(get_block_header(z), 0);
    make_block_free(z);
    void *y = use_block(get_unused(64), NULL, 64);
    if (x != y)
    {
        return 1;
    }
    return 0;
}

/**
 * Test galloc.h increase_strong_users.
 */
int galloc_test04()
{
    void *x = use_block(get_unused(64), NULL, 64);
    struct block *bx = get_block_header(x);
    uint32_t users[2];
    users[0] = get_ref_strong(bx);
    users[1] = get_ref_total(bx);
    if (users[0] == 0 || users[1] == 0)
    {
        return 1;
    }
    increase_strong_users(x);
    if (users[0] != get_ref_strong(bx) - 1 || users[1] != get_ref_total(bx) - 1)
    {
        return 1;
    }
    return 0;
}

/**
 * Test galloc.h increase_total_users.
 */
int galloc_test05()
{
    void *x = use_block(get_unused(64), NULL, 64);
    struct block *bx = get_block_header(x);
    uint32_t users[2];
    users[0] = get_ref_strong(bx);
    users[1] = get_ref_total(bx);
    if (users[0] == 0 || users[1] == 0)
    {
        return 1;
    }
    increase_total_users(x);
    if (users[0] != get_ref_strong(bx) || users[1] != get_ref_total(bx) - 1)
    {
        return 1;
    }
    return 0;
}

/**
 * Test galloc.h decrease_strong_users.
 */
int galloc_test06()
{
    void *x = use_block(get_unused(64), NULL, 64);
    struct block *bx = get_block_header(x);
    uint32_t users[2];
    users[0] = get_ref_strong(bx);
    users[1] = get_ref_total(bx);
    if (users[0] == 0 || users[1] == 0)
    {
        return 1;
    }
    decrease_strong_users(x);
    if (users[0] != get_ref_strong(bx) + 1 || users[1] != get_ref_total(bx) + 1)
    {
        return 1;
    }
    return 0;
}

/**
 * Test galloc.h decrease_total_users.
 */
int galloc_test07()
{
    void *x = use_block(get_unused(64), NULL, 64);
    struct block *bx = get_block_header(x);
    uint32_t users[2];
    users[0] = get_ref_strong(bx);
    users[1] = get_ref_total(bx);
    if (users[0] == 0 || users[1] == 0)
    {
        return 1;
    }
    decrease_total_users(x);
    if (users[0] != get_ref_strong(bx) || users[1] != get_ref_total(bx) + 1)
    {
        return 1;
    }
    return 0;
}

/**
 * Test galloc.h in_block.
 */
int galloc_test08()
{
    size_t len = 64;
    uint8_t *z = use_block(get_unused(len), NULL, len);
    if (!in_block(z, z + 0) || !in_block(z, z + len - 1) || in_block(z, z - 1) || in_block(z, z + len) || in_block(z, NULL))
    {
        return 1;
    }
    return 0;
}

/**
 * Test galloc.h.
 */
int galloc_tests(char *test)
{
    int results = 0;
    if (!strcmp(test, "galloc") || !strcmp(test, "galloc01"))
    {
        results |= galloc_test01();
    }
    
    if (!strcmp(test, "galloc") || !strcmp(test, "galloc02"))
    {
        results |= galloc_test02();
    }
    
    if (!strcmp(test, "galloc") || !strcmp(test, "galloc03"))
    {
        results |= galloc_test03();
    }
    
    if (!strcmp(test, "galloc") || !strcmp(test, "galloc04"))
    {
        results |= galloc_test04();
    }
    
    if (!strcmp(test, "galloc") || !strcmp(test, "galloc05"))
    {
        results |= galloc_test05();
    }
    
    if (!strcmp(test, "galloc") || !strcmp(test, "galloc06"))
    {
        results |= galloc_test06();
    }
    
    if (!strcmp(test, "galloc") || !strcmp(test, "galloc07"))
    {
        results |= galloc_test07();
    }
    
    if (!strcmp(test, "galloc") || !strcmp(test, "galloc08"))
    {
        results |= galloc_test08();
    }
    
    return results;
}
