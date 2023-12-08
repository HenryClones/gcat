#include "galloc.h"
#include "galloc_tests.h"
#include "string.h"

/**
 * Test galloc.h.
 */
int galloc_test01()
{
    return 0;
}

/**
 * Test galloc.h.
 */
int galloc_test02()
{
    return 0;
}

/**
 * Test galloc.h.
 */
int galloc_test03()
{
    return 0;
}

/**
 * Test galloc.h.
 */
int galloc_test04()
{
    return 0;
}

/**
 * Test galloc.h.
 */
int galloc_test05()
{
    return 0;
}

/**
 * Test galloc.h.
 */
int galloc_test06()
{
    return 0;
}

/**
 * Test galloc.h.
 */
int galloc_test07()
{
    return 0;
}

/**
 * Test galloc.h.
 */
int galloc_test08()
{
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
