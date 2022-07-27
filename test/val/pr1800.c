
/* Compiler outputs errors for valid code #1788 */
/* (rest tested later below) */
# /* Comment */

/* Unexpected "Error: Illegal indirection" under #if 0 #1769 */
#if 0
#if x ** 1
#endif
#endif

/* Unexpected "Error: Division by zero" #1768 */
#if 1 || (8 / 0)
#endif

/* Preprocessor fixes #1800 */

#/*
comment*/define __ATARI__ 1
# /*comment*/

#/*
*/define /**/M(/**/x, /*
*/y)/*
*/(x+/*
*/y)

#if M(-1, 1) != 0
#error M(x,y) error!
#endif

#warning/**//**/asd/**/)  fgh
#warning

#if 'z' - 'a' != 25
#if x ** 1
#endif
#endif

#if -1 < 0xF0000000 * 0
/* Note: this doesn't fail with cc65 as 0xF0000000 is an unsigned integer for cc65 */
#error "Fails with most non-cc65 compilers"
#endif

#if !+-0, (1U >> -31), x != (2L << -1)
#warning "/**/no problem"
#if 65536L * 32768L > -1U && 1 % 0
#error "error2"
#endif
#if -1 > 0U ? y != y : 1/0
#error "??"
#if d<(sasa
#endif
#endif
#endif

#if !defined(__ATARI__)
#error "__ATARI__" is undefined?!
#endif

#if !__ATARI__
#error "__ATARI__" == 0?!
#endif

#ifndef __ATARI__
#error __ATARI__ is undefined?!
#endif

int main(void)
{
    /* nope */
    return 0;
}
