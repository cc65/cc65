/*
  !!DESCRIPTION!! fseek test
  !!LICENCE!!     Public domain
*/

#include "common.h"

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

FILE *in;
char bufA[32];
char bufB[32];

#define INFILE   "cf.in"

int main(int argc,char **argv)
{
    static int r;

    in = fopen(INFILE, "rb");
    if (in == NULL) {
        return EXIT_FAILURE;
    }

    r = fread(bufA, 1, sizeof(bufA), in);

    if (r == 0) {
      printf("Error: could not read.\n");
      return EXIT_FAILURE;
    }
    fwrite(bufA, 1, r, stdout);

    /* Test SEEK_SET */
    fseek(in, 0, SEEK_SET);
    r = fread(bufB, 1, sizeof(bufB), in);

    if (r == 0) {
      printf("Error: could not re-read after SEEK_SET.\n");
      return EXIT_FAILURE;
    }
    fwrite(bufB, 1, r, stdout);

    if (memcmp(bufA, bufB, sizeof(bufA))) {
      printf("reads differ.\n");
      return EXIT_FAILURE;
    }

    /* Test SEEK_CUR */
    fseek(in, 0, SEEK_SET);
    fseek(in, 1, SEEK_CUR);
    r = fread(bufB, 1, sizeof(bufB), in);

    if (r == 0) {
      printf("Error: could not re-read after SEEK_CUR.\n");
      return EXIT_FAILURE;
    }
    fwrite(bufB, 1, r, stdout);

    if (memcmp(bufA+1, bufB, sizeof(bufA)-1)) {
      printf("reads differ.\n");
      return EXIT_FAILURE;
    }


    fclose(in);

    return 0;
}
