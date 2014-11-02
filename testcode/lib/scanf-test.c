/*
** scanf-test.c
**
** Tests that the scanf family of functions scans and converts its input data
** correctly.
**
** Note:  When this program uses conio, it doesn't guard against printing off
** the bottom of the screen.  So, you might have a problem on platforms with
** "short" screens.
**
** 2005-01-26, Greg King
*/

/* Define USE_STDIO, when you want to use the stdio functions.
** Do not define it, when you want to use the conio functions.
*/
/*
#define USE_STDIO
*/

#include <stdio.h>
#include <string.h>

#ifdef USE_STDIO
# define SCANF scanf
# define PRINTF printf

#else
# include <conio.h>

/* Unlike other conio input functions, cscanf() echoes what you type. */
# define SCANF cscanf
# define PRINTF cprintf
#endif

#define ARRAYSIZE(a) (sizeof (a) / sizeof (a)[0])

static const struct {
        const char *input, *format;
        int rvalue;
        enum TYPE {
                INT,
                CHAR
                } type1;
        union {
                int nvalue;
                const char *svalue;
                } v1;
        enum TYPE type2;
        union {
                int nvalue;
                const char *svalue;
                } v2;
        } test_data[] = {
/* Input sequences for character specifiers must be less than 80 characters
** long.  These format strings are allowwed a maximum of two assignment
** specifications.
*/
        /* Test that literals match, and that they aren't seen as conversions.
        ** Test that integer specifiers can handle end-of-file.
        */
        {"qwerty   Dvorak", "qwerty  Dvorak", 0  , INT, {0}, INT, {0}},
        {"qwerty"         , "qwerty  %d%i"  , EOF, INT, {0}, INT, {0}},
        {"qwerty   "      , "qwerty  %d%i"  , EOF, INT, {0}, INT, {0}},

        /* Test that integer specifiers scan properly. */
        {"qwerty   a"     , "qwerty  %d%i", 0, INT, {0}    , INT, {0}},
        {"qwerty   -"     , "qwerty  %d%i", 0, INT, {0}    , INT, {0}},
        {"qwerty   -9"    , "qwerty  %d%i", 1, INT, {-9}   , INT, {0}},
        {"qwerty   -95"   , "qwerty  %d%i", 1, INT, {-95}  , INT, {0}},
        {"qwerty   -95a"  , "qwerty  %d%i", 1, INT, {-95}  , INT, {0}},
        {"qwerty   -95a 1", "qwerty  %d%i", 1, INT, {-95}  , INT, {0}},
        {"qwerty   -a"    , "qwerty  %d%i", 0, INT, {0}    , INT, {0}},
        {"qwerty   -95 1" , "qwerty  %d%i", 2, INT, {-95}  , INT, {1}},
        {"qwerty    95  2", "qwerty  %i"  , 1, INT, {95}   , INT, {0}},
        {"qwerty   -95 +2", "qwerty  %x%o", 2, INT, {-0x95}, INT, {02}},
        {"qwerty  0X9E 02", "qwerty  %i%i", 2, INT, {0x9e} , INT, {2}},
        {"qwerty   095  2", "qwerty  %i%i", 2, INT, {0}    , INT, {95}},
        {"qwerty   0e5  2", "qwerty  %i%i", 1, INT, {0}    , INT, {0}},

        /* [String pointers are cast as (int),
        ** in order to avoid cc65 warnings.]
        */

        /* Test that character specifiers can handle end-of-file. */
        {"qwerty"   , "qwerty  %s%s"     , EOF, CHAR, {(int)""}, CHAR, {(int)""}},
        {"qwerty   ", "qwerty  %s%s"     , EOF, CHAR, {(int)""}, CHAR, {(int)""}},
        {"qwerty"   , "qwerty  %c%c"     , EOF, CHAR, {(int)""}, CHAR, {(int)""}},
        {"qwerty   ", "qwerty  %c%c"     , EOF, CHAR, {(int)""}, CHAR, {(int)""}},
        {"qwerty"   , "qwerty  %[ a-z]%c", EOF, CHAR, {(int)""}, CHAR, {(int)""}},
        {"qwerty   ", "qwerty  %[ a-z]%c", EOF, CHAR, {(int)""}, CHAR, {(int)""}},
        {"qwerty   ", "qwerty%s%s"       , EOF, CHAR, {(int)""}, CHAR, {(int)""}},

        /* Test that character specifiers scan properly. */
        {"123456qwertyasdfghzxcvbn!@#$%^QWERTYASDFGHZXCV"
            "BN7890-=uiop[]\\jkl;'m,./&*()_+UIOP{}|JKL:\"M<>?",
            "%79s%79s", 2,
            CHAR, {(int)"123456qwertyasdfghzxcvbn!@#$%^QWERTYASDFGHZXCV"
            "BN7890-=uiop[]\\jkl;'m,./&*()_+UIO"}, CHAR, {(int)"P{}|JKL:\"M<>?"}},
        {"qwerty   ", "qwerty%c%c"     , 2, CHAR, {(int)" "}  , CHAR, {(int)" "}},
        {"qwerty   ", "qwerty%2c%c"    , 2, CHAR, {(int)"  "} , CHAR, {(int)" "}},
        {"qwerty   ", "qwerty%2c%2c"   , 1, CHAR, {(int)"  "} , CHAR, {(int)" "}},
        {"qwerty   ", "qwerty%[ a-z]%c", 1, CHAR, {(int)"   "}, CHAR, {(int)""}},
        {"qwerty  q", "qwerty%[ a-z]%c", 1, CHAR, {(int)"  q"}, CHAR, {(int)""}},
        {"qwerty  Q", "qwerty%[ a-z]%c", 2, CHAR, {(int)"  "},  CHAR, {(int)"Q"}},
        {"qwerty-QWERTY-", "%[q-ze-]%[-A-Z]" , 2, CHAR, {(int)"qwerty-"},
            CHAR, {(int)"QWERTY-"}},

        /* Test the space-separation of strings. */
        {"qwerty qwerty"       , "qwerty%s%s", 1, CHAR, {(int)"qwerty"},
            CHAR, {(int)""}},
        {"qwerty qwerty Dvorak", "qwerty%s%s", 2, CHAR, {(int)"qwerty"},
            CHAR, {(int)"Dvorak"}},

        /* Test the mixxing of types. */
        {"qwerty  abc3", "qwerty%s%X"      , 1, CHAR, {(int)"abc3"}  , INT , {0}},
        {"qwerty  abc3", "qwerty%[ a-z]%X" , 2, CHAR, {(int)"  abc"} , INT , {3}},
        {"qwerty  abc3", "qwerty%[ a-z3]%X", 1, CHAR, {(int)"  abc3"}, INT , {0}},
        {"qwerty  abc3", "qwerty%[ A-Z]%X" , 2, CHAR, {(int)"  "}    , INT ,
            {0xabc3}},
        {"qwerty  3abc", "qwerty%i%[ a-z]" , 2, INT , {3}            , CHAR,
            {(int)"abc"}},
        {"qwerty  3abc", "qwerty%i%[ A-Z]" , 1, INT , {3}            , CHAR,
            {(int)""}},

        /* Test the character-count specifier. */
        {"  95 5", "%n%i"  , 1, INT , {0}          , INT, {95}},
        {"  a5 5", "%n%i"  , 0, INT , {0}          , INT, {0}},
        {"  a5 5", "%x%n"  , 1, INT , {0xa5}       , INT, {4}},
        {"  a5 5", " %4c%n", 1, CHAR, {(int)"a5 5"}, INT, {6}},
        {" 05a9" , "%i%n"  , 1, INT , {5}          , INT, {3}},

        /* Test assignment-suppression. */
        {"  95 6", "%n%*i"  , 0, INT , {0}      , INT, {0}},
        {"  a5 6", "%*x%n"  , 0, INT , {4}      , INT, {0}},
        {"  a5 6", "%*x%n%o", 1, INT , {4}      , INT, {6}},
        {"  a5 6", " %*4c%d", 0, CHAR, {(int)""}, INT, {0}},
        {"The first number is 7.  The second number is 8.\n",
            "%*[ .A-Za-z]%d%*[ .A-Za-z]%d", 2, INT, {7}, INT, {8}},
        };

/* Test the char, short, and long specification-modifiers. */
static const struct {
        const char *input, *format;
        long value;
        } type_data[] = {
        {"+123456789", "%hhd", (signed char)123456789L},
        {" 123456789", "%hd" , (unsigned short)123456789L},
        {" 123456789", "%ld" ,  123456789L},
        {"-123456789", "%lld", -123456789L},
        };

static void Pause(void) {
#ifdef USE_STDIO
        printf("\n");
#else
        cprintf("\r\nTap a key to see the next test. ");
        cgetc();
        clrscr();
#endif
        }

int main(void) {
        long n0;
        unsigned t;
        int c, n1 = 12345, n2, n3;
        char s1[80], s2[80];
        void *p1 = main, *p2 = main, *p3 = main, *p4 = main;

#ifndef USE_STDIO
        clrscr();
        cursor(1);
#endif

        /* Test that scanf() can recognize percent-signs in the input.
        ** Test that integer converters skip white-space.
        ** Test that "%i" can scan a single zero digit (followed by EOF).
        */
        sscanf("%  \r\n\f\v\t  0", "%%%i", &n1);
        if (n1 != 0)
                PRINTF("sscanf()'s \"%%%%%%i\" couldn't scan either a \"%%\" "
                       "or a single zero digit.\r\n\n");

        /* Test scanf()'s return-value:  EOF if input ends before the first
        ** conversion-attempt begins; an assignment-count, otherwise.
        ** Test that scanf() properly converts and assigns the correct number
        ** of arguments.
        */
        PRINTF("Testing scanf()'s return-value,\r\nconversions, and assignments...\r\n");
        for (t = 0; t < ARRAYSIZE(test_data); ++t) {

                /* Prefill the arguments with zeroes. */
                n1 = n2 = 0;
                memset(s1, '\0', sizeof s1);
                memset(s2, '\0', sizeof s2);

                c=sscanf(test_data[t].input, test_data[t].format,
                         /* Avoid warning messages about different
                         ** pointer-types, by casting them to void-pointers.
                         */
                         test_data[t].type1 == INT ? (void *)&n1 : (void *)s1,
                         test_data[t].type2 == INT ? (void *)&n2 : (void *)s2);
                if (c != test_data[t].rvalue)
                        PRINTF("Test #%u returned %d instead of %d.\r\n",
                               t + 1, c, test_data[t].rvalue);

                if (test_data[t].type1 == INT) {
                        if (test_data[t].v1.nvalue != n1)
                                PRINTF("Test #%u assigned %i, instead of %i,\r\n"
                                       "\tto the first argument.\r\n\n",
                                       t + 1, n1, test_data[t].v1.nvalue);
                        }
                else {          /* test_data[t].type1 == CHAR */
                        if (strcmp(test_data[t].v1.svalue, s1))
                                PRINTF("Test #%u assigned\r\n\"%s\",\r\n"
                                       "\tinstead of\r\n\"%s\",\r\n"
                                       "\tto the first argument.\r\n\n",
                                       t + 1, s1, test_data[t].v1.svalue);
                        }

                if (test_data[t].type2 == INT) {
                        if (test_data[t].v2.nvalue != n2)
                                PRINTF("Test #%u assigned %i, instead of %i,\r\n"
                                       "\tto the second argument.\r\n\n",
                                       t + 1, n2, test_data[t].v2.nvalue);
                        }
                else {          /* test_data[t].type2 == CHAR */
                        if (strcmp(test_data[t].v2.svalue, s2))
                                PRINTF("Test #%u assigned\r\n\"%s\",\r\n"
                                       "\tinstead of\r\n\"%s\",\r\n"
                                       "\tto the second argument.\r\n\n",
                                       t + 1, s2, test_data[t].v2.svalue);
                        }
                }
        Pause();

        /* Test the char, short, and long specification-modifiers. */
        PRINTF("Testing scanf()'s type-modifiers...\r\n");
        for (t = 0; t < ARRAYSIZE(type_data); ++t) {
                n0 = 0L;
                sscanf(type_data[t].input, type_data[t].format, &n0);
                if (type_data[t].value != n0)
                        PRINTF("Test #%u assigned %li instead of %li.\r\n",
                               t + 1, n0, type_data[t].value);
                }
        Pause();

        /* Test that the pointer specification
        ** can convert what printf() generates.
        */
        PRINTF("Testing \"%%p\"...\r\n");
        sprintf(s1, "%p %p %p %p", NULL, NULL,
                Pause,                  /* static (program) storage */
                &c);                    /* automatic (stack) storage */
        sscanf(s1, "%p%p%p %p", &p1, &p2, &p3, &p4);
        if (p1 != NULL || p2 != NULL ||
            p3 != (void *)Pause || p4 != (void *)&c)
                PRINTF("p1 is %p, p2 is %p; they should be %p.\r\n"
                       "scanf() assigned %p to p3, instead of %p.\r\n"
                       "scanf() assigned %p to p4, instead of %p.\r\n",
                       p1, p2, NULL,
                       p3, Pause,
                       p4, &c);

        /* Test that scanf() can scan typed input.
        ** Retest that "%i" can decode radix prefixxes.
        */
        do {
                Pause();
                PRINTF("Type 3 signed numbers,\r\n"
                       "separated by white-space:\r\n"
                       "octal decimal hexadecimal\r\n"
                       "? ");
                c = SCANF("%i %i %i", &n1, &n2, &n3);
                PRINTF("\r\n\nscanf() returned %i.\r\n"
                       "The numbers are:\r\n"
                       " %+o octal,\r\n"
                       " %+d decimal,\r\n"
                       " %+#X hexadecimal.\r\n",
                       c, n1, n2, n3);
                } while (c > 0);
        return 0;
        }
