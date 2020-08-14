
/* bug #1181 - Testing struct member against NULL is broken */ 

#include <stdio.h>
#include <stdlib.h>

struct {
    int a;
} s = { 256 }, *ps = &s;

int res = EXIT_SUCCESS;

void test1(void)
{
    if (ps->a) {
        printf("OK\n");
    } else {
        printf("ERROR: %d\n", ps->a);
        res = EXIT_FAILURE;
    }
}

typedef struct _MENUITEM
{
    char *name;
} MENUITEM;

typedef struct _MENU
{
    struct _MENUITEM *items;
} MENU;

/* note: the behaviour changes when these strings are changed! */
static unsigned char oi31[] = {"Browser Exec Setup"};
static unsigned char oi36[] = {"Browser auto sort"};
static unsigned char oi47[] = {"Browser startup"};
static unsigned char oi49[] = {"Browser charset"};
static unsigned char oi55[] = {"Menu color scheme"};
static unsigned char oi63[] = {"Menu input scheme"};
static unsigned char oi35[] = {"back"};

MENUITEM optionsitems_menu[] = {
        {oi31},
        {oi36},
        {oi47},
        {oi49},
        {oi55},
        {oi63},
        {oi35},
        {NULL}
};

static MENU optionsmenu_menu = {
        &optionsitems_menu[0],
}; 

unsigned char __fastcall__ menu_getnumitems(MENU *menu)
{
static unsigned char numitems;
MENUITEM *items;
    numitems = 0;
    items = menu->items;
    while(items->name)
    {
        ++numitems;
        ++items;
    }
    return numitems;
}

int main(void)
{
    unsigned char i = 0;

    i = menu_getnumitems(&optionsmenu_menu);
    printf("numitems (expected 7): %d\n", i);

    if (i != 7) {
        printf("failed\n");
        res = EXIT_FAILURE;
    }
    printf("passed\n");

    test1();
    return res;
}
