
/*
  !!DESCRIPTION!! forgetting to emit labels
  !!ORIGIN!!      Testsuite
  !!LICENCE!!     Public Domain
*/

/*
    http://www.cc65.org/mailarchive/2014-10/11673.html
    http://www.cc65.org/mailarchive/2014-10/11675.html
*/

#include <stdlib.h>
#include <stdio.h>

struct udata {
        int     (*u_sigvec[16])();
        int     u_argn;
        int     u_argn1;
};

struct udata udata;

#define sig (int)udata.u_argn
#define func (int (*)())udata.u_argn1

int _signal(void)
{
        if (func != 0) {
                goto nogood;
        }
        udata.u_sigvec[sig] = func;
        return 0;

nogood:
        return (-1);
}

int main(int n,char **args)
{
    _signal();
    printf("it works\n");

    return 0;
}
