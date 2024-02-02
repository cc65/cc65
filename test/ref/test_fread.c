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
char buf[32];

#define INFILE   "cf.in"

int main(int argc,char **argv)
{
    static char outfile_path[FILENAME_MAX+1];
    static int r;

    sprintf(outfile_path, "%s.test.out", argv[0]);

    out = fopen(outfile_path, "wb");
    if (out == NULL) {
        return EXIT_FAILURE;
    }
    if (fread(buf, 1, sizeof(buf), out) != NULL) {
      printf("Error, could fread with write-only file\n");
      return 1;
    }
    if (!ferror(out)) {
      printf("Error: file pointer should be in error state\n");
    }
    fclose(out);
    unlink(outfile_path);

    in = fopen(INFILE, "rb");
    if (in == NULL) {
        return EXIT_FAILURE;
    }

    /* Test that ungetc doesn't break fread */
    buf[0] = fgetc(in);
    ungetc(buf[0], in);

    r = fread(buf, 1, sizeof(buf), out);

    if (r == 0) {
      printf("Error: could not start reading.\n");
    }
    fwrite(buf, 1, r, stdout);

    /* Finish reading file. */
    while ((r = fread(buf, 1, sizeof(buf), out)) != 0)
    {
        fwrite(buf, 1, r, stdout);
    }

    if (!feof(in))
    {
        printf("We should have EOF!\n");
    }

    fclose(in);
    return 0;
}
