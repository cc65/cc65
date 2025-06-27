/*
** !!DESCRIPTION!! Simple #pragma once directive tests
** !!ORIGIN!!      cc65 regression tests
** !!LICENCE!!     Public Domain
*/

#pragma once

#include "pragma-once-sample-2.h"
#include "pragma-once-sample-2.h"
#include "pragma-once-sample.h"
#include "pragma-once-sample.h"


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
