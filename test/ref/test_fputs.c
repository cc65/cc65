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
char buf[512], err;

#define INFILE   "cf.in"

int main(int argc,char **argv)
{
    in = fopen(INFILE, "rb");
    if (in == NULL) {
        return EXIT_FAILURE;
    }

    strcpy(buf, "test");
    if (fputs(buf, in) != EOF) {
      printf("Error: can fputs to a file opened for reading\n");
      return EXIT_FAILURE;
    }
    clearerr(in);

    while (fgets(buf, 512, in) != NULL) {
      fputs(buf, stdout);
    }

    fclose(in);
    return 0;
}
