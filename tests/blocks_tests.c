#include <string.h>
#include "blocks.h"
#include "blocks_tests.h"

/**
 * Test blocks.h set_flag.
 */
static int blocks_test01()
{
    return 0;
}

/**
 * Test blocks.h set_flag with no next node.
 */
static int blocks_test02()
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
static int blocks_test03()
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
static int blocks_test04()
{
    struct block x;
    size_t y = 100;
    set_size(&x, y);
    if (get_size(&x) != y)
    {
        return 1;
    }
    
    y = 50;

    set_size(&x, y);
    if (get_size(&x) != y)
    {
        return 1;
    }
    
    return 0;
}

/**
 * Test blocks.h set_prev.
 */
static int blocks_test05()
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
static int blocks_test06()
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
static int blocks_test07()
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
static int blocks_test08()
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
static int blocks_test09()
{
    struct block *xp;
    char buf[128];
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
static int blocks_test10()
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
static int blocks_test11()
{
    struct block x;
    set_finalizer(&x, finalizer);
    struct block z = *(free_block(&x, NULL));
    if (!finalizer_ran)
    {
        return 1;
    }
    finalizer_ran = !finalizer_ran;
    struct block y;
    set_flag(&y, unused, 0);
    set_finalizer(&x, finalizer);
    z = *(free_block(&x, &y));
    if (!finalizer_ran || get_prevflag(&y) != unused || &z != &x)
    {
        return 1;
    }
    return 0;
}

/**
 * Test blocks.h set_flag.
 */
static int blocks_test12()
{
    return 0;
}

/**
 * Test blocks.h set_flag.
 */
static int blocks_test13()
{
    return 0;
}

/**
 * Test blocks.h set_flag.
 */
static int blocks_test14()
{
    return 0;
}

/**
 * Test blocks.h set_flag.
 */
static int blocks_test15()
{
    return 0;
}

/**
 * Test blocks.h set_flag.
 */
static int blocks_test16()
{
    return 0;
}

/**
 * Test blocks.h set_flag.
 */
static int blocks_test17()
{
    return 0;
}

/**
 * Test blocks.h set_flag.
 */
int blocks_tests(char *test)
{
    int results = 0;
    if (!strcmp(test, "blocks") && !strcmp(test, "blocks01"))
    {
        results |= blocks_test01();
    }
    
    if (!strcmp(test, "blocks") && !strcmp(test, "blocks02"))
    {
        results |= blocks_test02();
    }
    
    if (!strcmp(test, "blocks") && !strcmp(test, "blocks03"))
    {
        results |= blocks_test03();
    }
    
    if (!strcmp(test, "blocks") && !strcmp(test, "blocks04"))
    {
        results |= blocks_test04();
    }
    
    if (!strcmp(test, "blocks") && !strcmp(test, "blocks05"))
    {
        results |= blocks_test05();
    }
    
    if (!strcmp(test, "blocks") && !strcmp(test, "blocks06"))
    {
        results |= blocks_test06();
    }
    
    if (!strcmp(test, "blocks") && !strcmp(test, "blocks07"))
    {
        results |= blocks_test07();
    }
    
    if (!strcmp(test, "blocks") && !strcmp(test, "blocks08"))
    {
        results |= blocks_test08();
    }
    
    if (!strcmp(test, "blocks") && !strcmp(test, "blocks09"))
    {
        results |= blocks_test09();
    }
    
    if (!strcmp(test, "blocks") && !strcmp(test, "blocks10"))
    {
        results |= blocks_test10();
    }
    
    if (!strcmp(test, "blocks") && !strcmp(test, "blocks11"))
    {
        results |= blocks_test11();
    }
    
    if (!strcmp(test, "blocks") && !strcmp(test, "blocks12"))
    {
        results |= blocks_test12();
    }
    
    if (!strcmp(test, "blocks") && !strcmp(test, "blocks13"))
    {
        results |= blocks_test13();
    }
    
    if (!strcmp(test, "blocks") && !strcmp(test, "blocks14"))
    {
        results |= blocks_test14();
    }
    
    if (!strcmp(test, "blocks") && !strcmp(test, "blocks15"))
    {
        results |= blocks_test15();
    }
    
    if (!strcmp(test, "blocks") && !strcmp(test, "blocks16"))
    {
        results |= blocks_test16();
    }
    
    return 0;
}
