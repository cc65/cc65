/*
  !!DESCRIPTION!! fgets test
  !!LICENCE!!     Public domain
*/

#include "common.h"

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

FILE *in, *out;
char buf[32];

#define INFILE   "cf.in"
#define OUTFILE  "test_fgets.out"

#ifdef NO_OLD_FUNC_DECL
int main(int argc,char **argv)
#else
main(argc, argv)
int argc;
char *argv[];
#endif
{
    out = fopen(OUTFILE, "wb");
    if (out == NULL) {
        return EXIT_FAILURE;
    }
    if (fgets(buf, sizeof(buf), out) != NULL) {
      printf("Error, could fgets with write-only file\n");
      return 1;
    }
    if (!ferror(out)) {
      printf("Error: file pointer should be in error state\n");
    }
    fclose(out);

    in = fopen(INFILE, "rb");
    if (in == NULL) {
        return EXIT_FAILURE;
    }

    if (fgets(NULL, 0, in) != NULL) {
      printf("Error, could fgets with zero size\n");
      return 1;
    }

    /* Test ungetc while we're at it */
    buf[0] = fgetc(in);
    ungetc(buf[0], in);


    while (fgets(buf, sizeof(buf), in) != NULL)
    {
        printf("%s",buf);
    }
    if (!feof(in))
    {
        printf("We should have EOF!\n");
    }
    fclose(in);
    return 0;
}
