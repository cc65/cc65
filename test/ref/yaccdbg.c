/*
  !!DESCRIPTION!! verbose/debug version of yacc.c (if one fails and the other does not you most likely have a stack related problem)
  !!ORIGIN!!      LCC 4.1 Testsuite
  !!LICENCE!!     own, freely distributeable for non-profit. read CPYRIGHT.LCC
*/

/*#define STANDALONE*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define INFILE   "yaccdbg.in"

#define LEXDEBUG
#define YYDEBUG

#define YACCDBG
#include "yacc.c"
