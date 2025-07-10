/*
  !!DESCRIPTION!! fgets test
  !!LICENCE!!     Public domain
*/

#include "common.h"

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

FILE *in, *out;
int c, err;

#define INFILE   "cf.in"

int main(int argc,char **argv)
{
    in = fopen(INFILE, "rb");
    if (in == NULL) {
        return EXIT_FAILURE;
    }

    if (fputc(c, in) != EOF) {
      printf("Error: can fputc to a file opened for reading\n");
      return EXIT_FAILURE;
    }
    clearerr(in);

    while ((c = fgetc(in)) != EOF) {
      fputc(c, stdout);
    }

    fclose(in);
    return 0;
}
