/*
** !!DESCRIPTION!! Simple #pragma once directive tests
** !!ORIGIN!!      cc65 regression tests
** !!LICENCE!!     Public Domain
*/


#include "pragma-once-sample.h"
#include "pragma-once-sample.h"
#include "pragma-once-sample-link.h"

/* pragma-once-sample-link.h is a symlink to pragma-once-sample. */


#include <stdio.h>


int main() {

#ifdef INCLUDED_TWICE
    printf("pragma-once-sample.h included more than once\n");
    return 1;
#else
    printf("pragma-once-sample included once\n");
    return 0;
#endif
}
