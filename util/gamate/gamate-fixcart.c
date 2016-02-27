
#include <stdlib.h>
#include <stdio.h>

FILE *in;
unsigned int n, i, c;

void usage(char *arg)
{
    printf("usage: %s [file]\n", arg);
    exit(-1);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        usage(argv[0]);
        exit(-1);
    }

    if (!(in = fopen(argv[1], "r+b"))) {
        fprintf(stderr, "couldnt open: '%s'\n", argv[1]);
        exit(-1);
    }
    /* read 0x1000 bytes from 0x7000-0x7fff (offset 0x1000) */
    fseek(in, 0x1000, SEEK_SET);
    n = 0; for (i = 0; i < 0x1000; i++) {
        c =  fgetc(in);
        n += c;
    }
    /* write checksum to header */
    fseek(in, 0, SEEK_SET);
    fputc(n & 0xff, in);
    fputc((n >> 8) & 0xff, in);

    fclose(in);
    return (0);
}
