/*
  !!DESCRIPTION!! print character frequencies
  !!ORIGIN!!      LCC 4.1 Testsuite
  !!LICENCE!!     own, freely distributeable for non-profit. read CPYRIGHT.LCC
*/

/*
        cf - print character frequencies
*/

#include "common.h"

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

FILE *in;

#define INFILE   "cf.in"
#define GETCHAR() fgetc(in)

#ifndef NO_FLOATS
float f[0x100];
#else
signed f[0x100];
#endif

#ifdef NO_OLD_FUNC_DECL
int main(int argc,char **argv)
#else
main(argc, argv)
int argc;
char *argv[];
#endif
{
	int i, c, nc;
#ifndef NO_FLOATS
	float cutoff, atof();
#else
        signed cutoff;
#endif

        in = fopen(INFILE, "rb");
        if (in == NULL) {
            return EXIT_FAILURE;
        }

	if (argc <= 1)
#ifndef NO_FLOATS
		cutoff = 0.0;
#else
                cutoff = 0;
#endif
	else
#ifndef NO_FLOATS
		cutoff = atof(argv[1])/100;
#else
                cutoff = atoi(argv[1])/100;
#endif
	for (i = 0; i < 0x100; )
    {
#ifndef NO_FLOATS
		f[i++] = 0.0;
#else
        f[i++] = 0;
#endif
    }

    printf("input:\n\n");
    
    nc = 0;
    while ((c = GETCHAR()) != -1)
    {
/*        printf("[%02x]",c); */
        printf("%c",c);
        f[c] += 1;
        nc++;
    }
    printf("\n\ncount: %d\n\n",nc);

    /*
        now try to print a report in a way so that
        the order is somewhat independent from the
        target character set
    */

    printf("a-z char:freq\n\n");

    /* first round ... lowercase characters */
	for (i = 0; i < 0x100; ++i)
    {
		if ((f[i]) && ((f[i]/nc) >= cutoff))
        {
			if ((i >= 'a') && (i <= 'z'))
            {
				printf("%c", i);
#ifndef NO_FLOATS
                printf(":%.1f\n", 100*f[i]/nc);
#else
                printf(":%d\n", 100*f[i]/nc);
#endif
                f[i]=0;
            }
		}
    }

    printf("A-Z char:freq\n\n");

    /* second round ... uppercase characters */
	for (i = 0; i < 0x100; ++i)
    {
		if ((f[i]) && ((f[i]/nc) >= cutoff))
        {
			if ((i >= 'A') && (i <= 'Z'))
            {
				printf("%c", i);
#ifndef NO_FLOATS
                printf(":%.1f\n", 100*f[i]/nc);
#else
                printf(":%d\n", 100*f[i]/nc);
#endif
                f[i]=0;
            }
		}
    }

    printf("0-9 char:freq\n\n");

    /* third round ... numbers */
	for (i = 0; i < 0x100; ++i)
    {
		if ((f[i]) && ((f[i]/nc) >= cutoff))
        {
			if ((i >= '0') && (i <= '9'))
            {
				printf("%c", i);
#ifndef NO_FLOATS
                printf(":%.1f\n", 100*f[i]/nc);
#else
                printf(":%d\n", 100*f[i]/nc);
#endif
                f[i]=0;
            }
		}
    }

    printf("isprint char:freq\n\n");

    /* second last round ... remaining printable characters */
	for (i = 0; i < 0x100; ++i)
    {
		if ((f[i]) && ((f[i]/nc) >= cutoff))
        {
			if(isprint(i))
            {
				printf("%c", i);
#ifndef NO_FLOATS
                printf(":%.1f\n", 100*f[i]/nc);
#else
                printf(":%d\n", 100*f[i]/nc);
#endif
                f[i]=0;
            }
		}
    }

    printf("rest char:freq\n\n");

    /* last round ... remaining non printable characters */
	for (i = 0; i < 0x100; ++i)
    {
		if ((f[i]) && ((f[i]/nc) >= cutoff))
        {
            if(i=='\n')
            {
				printf("newline");
            }
            else
            {
				printf("%03o", i);
            }
#ifndef NO_FLOATS
                printf(":%.1f\n", 100*f[i]/nc);
#else
                printf(":%d\n", 100*f[i]/nc);
#endif
		}
    }
    fclose(in);
    return 0;
}
