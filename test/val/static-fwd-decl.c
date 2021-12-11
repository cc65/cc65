/*
  !!DESCRIPTION!! static forward declarations
  !!ORIGIN!!      cc65 regression tests
  !!LICENCE!!     Public Domain
  !!AUTHOR!!      Bob Andrews
*/

/*
  see: https://github.com/cc65/cc65/issues/204
*/

#pragma warn(error, on)

typedef struct _DIRMENU
{
    const char *name;
    struct _DIRMENU *dest;
} DIRMENU; 

static DIRMENU rmenu;

static DIRMENU lmenu = {
    "left",
    &rmenu
};

static DIRMENU rmenu = {
    "right",
    &lmenu
};

int main(void)
{
    return lmenu.dest == &rmenu && rmenu.dest == &lmenu ? 0 : 1;
}
