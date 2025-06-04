/*
** !!DESCRIPTION!! Simple #pragma once directive tests
** !!ORIGIN!!      cc65 regression tests
** !!LICENCE!!     Public Domain
*/

#ifdef FILE_INCLUDED

#error "This file should not have been included twice"
#define INCLUDED_TWICE

#else

#define FILE_INCLUDED

#endif


/* a pragma once directive should work regardless of where it is located in
   the file, as long as it is seen by the preprocessor */
#pragma once
