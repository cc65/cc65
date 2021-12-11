
/*
    bug #1552 - crash in fuzix xec.c

    cc65 -t none -O bug1552.c
*/

#include <stdio.h>

typedef struct trenod *TREPTR;
typedef struct whnod *WHPTR;

struct trenod {
    int tretyp;
};

struct whnod {
    int whtyp;
    TREPTR whtre;
};

int execute(TREPTR argt, int execflg, int *pf1, int *pf2)
{
    register TREPTR t;
    int type;
    switch (type) 
    {
        case 6:
        {
            while ((execute(((WHPTR) t)->whtre, 0, NULL, NULL) == 0) == (type == 5)) {

            }
            break;
        }
    }
    return 0;
}

int main(void)
{
    return execute((TREPTR)42, 2, (int *)3, (int *)4);
}
