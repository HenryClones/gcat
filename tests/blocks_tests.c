#include <string.h>
#include "blocks.h"
#include "blocks_tests.h"

/**
 * Test blocks.h set_flag with no next node.
 */
static int blocks_test01()
{
    // 1a. set value
    struct block x;
    set_flag(&x, unused, 0);
    if (get_flag(&x) != unused)
    {
        return 1;
    }
    // 1b. go back
    set_flag(&x, used, 0);
    if (get_flag(&x) != used)
    {
        return 1;
    }

    return 0;
}

/**
 * Test blocks.h set_flag with a next node.
 */
static int blocks_test02()
{
    // 2a: next=true
    struct block x;
    struct block y;
    set_next(&x, &y);
    set_flag(&x, unused, 1);
    if (get_flag(&x) != unused)
    {
        return 1;
    }
    // 2b: go back
    set_flag(&x, used, 1);
    if (get_flag(&x) != used)
    {
        return 1;
    }

    return 0;
}

/**
 * Test blocks.h set_size.
 */
static int blocks_test03()
{
    struct block x;
    size_t y = 100;
    set_flag(&x, used, 0);
    set_size(&x, y);
    if (get_size(&x) < y)
    {
        return 1;
    }
    
    y = 50;

    set_size(&x, y);
    if (get_size(&x) < y)
    {
        return 1;
    }
    
    return 0;
}

/**
 * Test blocks.h set_prev.
 */
static int blocks_test04()
{
    struct block x;
    struct block y;
    set_prev(&x, &y);
    if (get_prev(&x) != &y)
    {
        return 1;
    }
    set_prev(&x, NULL);
    if (get_prev(&x) != NULL)
    {
        return 1;
    }
    return 0;
}

/**
 * Test blocks.h set_next.
 */
static int blocks_test05()
{
    struct block x;
    struct block y;
    set_next(&x, &y);
    if (get_next(&x) != &y)
    {
        return 1;
    }
    set_next(&x, NULL);
    if (get_next(&x) != NULL)
    {
        return 1;
    }
    return 0;
}

/**
 * Test blocks.h get_ref_strong.
 */
static int blocks_test06()
{
    struct block x;
    struct block y;

    int refs = 0;

    update_ref_strong(&x, -get_ref_strong(&x) + refs);
    if (get_ref_strong(&x) != refs)
    {
        return 1;
    }
    refs = 10;
    update_ref_strong(&x, refs);
    if (get_ref_strong(&x) != refs)
    {
        return 1;
    }
    return 0;
}

/**
 * Test blocks.h get_ref_total.
 */
static int blocks_test07()
{
    struct block x;
    struct block y;

    int refs = 0;

    update_ref_total(&x, -get_ref_total(&x) + refs);
    if (get_ref_total(&x) != refs)
    {
        return 1;
    }
    refs = 10;
    update_ref_total(&x, refs);
    if (get_ref_total(&x) != refs)
    {
        return 1;
    }
    return 0;
}

/**
 * Test blocks.h get_payload.
 */
static int blocks_test08()
{
    struct block *xp;
    uint8_t buf[128]__attribute__((aligned));
    int len = 80;
    uint8_t y = 40;
    xp = (struct block *) buf;
    set_size(xp, len);
    uint8_t *payload = (uint8_t *) get_payload(xp);
    payload[0] = y;
    payload[len - 1] = y;
    if (payload[len - 1] != payload[0])
    {
        return 1;
    }
    if (payload != (uint8_t *) get_payload(xp))
    {
        return 1;
    }
    return 0;
}

static int finalizer_ran = 0;

static void finalizer(void *payload)
{
    finalizer_ran = !finalizer_ran;
}

/**
 * Test blocks.h set_finalizer.
 */
static int blocks_test09()
{
    struct block x;
    set_finalizer(&x, finalizer);
    if (get_finalizer(&x) != finalizer)
    {
        return 1;
    }
    set_finalizer(&x, NULL);
    if (get_finalizer(&x) != NULL)
    {
        return 1;
    }
    return 0;
}

/**
 * Test blocks.h free_block.
 */
static int blocks_test10()
{
    struct block x[2];
    struct block *y = &(x[1]);
    set_finalizer(x, finalizer);
    struct block *z = free_block(x, x, 1);
    if (!finalizer_ran)
    {
        return 1;
    }
    finalizer_ran = !finalizer_ran;
    set_flag(y, unused, 0);
    z = free_block(y, x, 0);
    if (!finalizer_ran || get_prevflag(y) != unused || z != x)
    {
        return 1;
    }
    return 0;
}

/**
 * Test blocks.h get_after.
 */
static int blocks_test11()
{
    // 1. Minimum size
    struct block x[2];
    struct block *px = &(x[0]);
    struct block *py = &(x[1]);
    set_size(px, 1);
    set_size(py, 1);
    if (get_after(px) != py)
    {
        return 1;
    }
    // 2. Bigger blocks
    uint8_t buf[512]__attribute__((aligned));
    px = (struct block *)buf;
    size_t size_x = 230;
    set_size(px, size_x);
    py = get_after(px);
    size_t size_y = 27;
    set_size(py, size_y);
    if (get_after(px) != py || py < px + size_x + sizeof(*px) - sizeof(px->payload))
    {
        return 1;
    }
    return 0;
}

/**
 * Test blocks.h get_before.
 */
static int blocks_test12()
{
    // 1a. Minimum size, used
    struct block x;
    struct block y;
    struct block *px = &x;
    struct block *py = &y;
    set_flag(px, used, 0);
    set_size(px, 1);
    free_block(px, px, 1);
    set_flag(py, used, 0);
    set_size(py, 1);
    if (get_before(py) != NULL)
    {
        return 1;
    }
    free_block(py, px, 0);
    if (get_before(py) != px)
    {
        return 1;
    }
    return 0;
}

/**
 * Test blocks.h get_full_size.
 */
static int blocks_test13()
{
    struct block x;
    size_t size = 300;
    set_size(&x, size);
    struct block *py;
    uint8_t buf[400];
    py = (struct block *) buf;
    set_size(py, size);
    struct block *pz = get_after(py);
    set_size(pz, 10);
    return block_full_size(&x) != block_full_size(py) || block_full_size(pz) == block_full_size(&x);
}

/**
 * Test blocks.h get_block_boundary.
 */
static int blocks_test14()
{
    uint8_t buf[30];
    struct block *px;
    px = (struct block *) buf;
    set_size(px, sizeof(size_t));
    return get_block_boundary(px) != (size_t *) px->payload;
}

/**
 * Test blocks.h.
 */
int blocks_tests(char *test)
{
    int results = 0;
    if (!strcmp(test, "blocks") || !strcmp(test, "blocks01"))
    {
        results |= blocks_test01();
    }
    
    if (!strcmp(test, "blocks") || !strcmp(test, "blocks02"))
    {
        results |= blocks_test02();
    }
    
    if (!strcmp(test, "blocks") || !strcmp(test, "blocks03"))
    {
        results |= blocks_test03();
    }
    
    if (!strcmp(test, "blocks") || !strcmp(test, "blocks04"))
    {
        results |= blocks_test04();
    }
    
    if (!strcmp(test, "blocks") || !strcmp(test, "blocks05"))
    {
        results |= blocks_test05();
    }
    
    if (!strcmp(test, "blocks") || !strcmp(test, "blocks06"))
    {
        results |= blocks_test06();
    }
    
    if (!strcmp(test, "blocks") || !strcmp(test, "blocks07"))
    {
        results |= blocks_test07();
    }
    
    if (!strcmp(test, "blocks") || !strcmp(test, "blocks08"))
    {
        results |= blocks_test08();
    }
    
    if (!strcmp(test, "blocks") || !strcmp(test, "blocks09"))
    {
        results |= blocks_test09();
    }
    
    if (!strcmp(test, "blocks") || !strcmp(test, "blocks10"))
    {
        results |= blocks_test10();
    }
    
    if (!strcmp(test, "blocks") || !strcmp(test, "blocks11"))
    {
        results |= blocks_test11();
    }
    
    if (!strcmp(test, "blocks") || !strcmp(test, "blocks12"))
    {
        results |= blocks_test12();
    }
    
    if (!strcmp(test, "blocks") || !strcmp(test, "blocks13"))
    {
        results |= blocks_test13();
    }
    
    if (!strcmp(test, "blocks") || !strcmp(test, "blocks14"))
    {
        results |= blocks_test14();
    }
    
    return results;
}
