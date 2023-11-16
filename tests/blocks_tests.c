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
 * Test blocks.h set_flag.
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

    // 2a: next=true
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
 * Test blocks.h set_flag.
 */
static int blocks_test03()
{
    return 0;
}

/**
 * Test blocks.h set_flag.
 */
static int blocks_test04()
{
    return 0;
}

/**
 * Test blocks.h set_flag.
 */
static int blocks_test05()
{
    return 0;
}

/**
 * Test blocks.h set_flag.
 */
static int blocks_test06()
{
    return 0;
}

/**
 * Test blocks.h set_flag.
 */
static int blocks_test07()
{
    return 0;
}

/**
 * Test blocks.h set_flag.
 */
static int blocks_test08()
{
    return 0;
}

/**
 * Test blocks.h set_flag.
 */
static int blocks_test09()
{
    return 0;
}

/**
 * Test blocks.h set_flag.
 */
static int blocks_test10()
{
    return 0;
}

/**
 * Test blocks.h set_flag.
 */
static int blocks_test11()
{
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
