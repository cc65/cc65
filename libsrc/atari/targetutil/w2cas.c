/* w2cas.c -- write file to cassette
**
** This program writes a boot file (typically linked with
** 'atari-cassette.cfg') to the cassette.
** Only files < 32K are supported, since the loading of
** larger files requires a special loader inside the program.
**
** Christian Groessler, chris@groessler.org, 2014
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <6502.h>
#include <atari.h>
#include <cc65.h>
#include <conio.h>

static int verbose = 1;
static char C_dev[] = "C:";

static struct __iocb *findfreeiocb(void)
{
    int i;

    for (i = 0; i < 8; i++) {
        if (OS.iocb[i].handler == 0xff)
            return &OS.iocb[i];
    }
    return NULL;
}

int main(int argc, char **argv)
{
    char *filename, *x;
    char buf[20];
    FILE *file;
    unsigned char *buffer;
    size_t filen, buflen = 32768l + 1;
    struct regs regs;
    struct __iocb *iocb = findfreeiocb();
    int iocb_num;

    /* if DOS will automatically clear the screen after the program exits, wait for a keypress... */
    if (doesclrscrafterexit())
        atexit((void (*)(void))cgetc);

    if (! iocb) {
        fprintf(stderr, "couldn't find a free iocb\n");
        return 1;
    }
    iocb_num = (iocb - OS.iocb) * 16;
    if (verbose)
        printf("using iocb index $%02X ($%04X)\n", iocb_num, iocb);

    if (argc < 2) {
        printf("\nfilename: ");
        x = fgets(buf, 19, stdin);
        printf("\n");
        if (! x) {
            printf("empty filename, exiting...\n");
            return 1;
        }
        if (*x && *(x + strlen(x) - 1) == '\n')
            *(x + strlen(x) - 1) = 0;
        if (! strlen(x)) {  /* empty filename */
            printf("empty filename, exiting...\n");
            return 1;
        }
        filename = x;
    }
    else {
        filename = *(argv+1);
    }

    /* allocate buffer */
    buffer = malloc(buflen);
    if (! buffer) {
        buflen = _heapmaxavail(); /* get as much as we can */
        buffer = malloc(buflen);
        if (! buffer) {
            fprintf(stderr, "cannot alloc %ld bytes -- aborting...\n", (long)buflen);
            return 1;
        }
    }
    if (verbose)
        printf("buffer size: %ld bytes\n", (long)buflen);

    /* open file */
    file = fopen(filename, "rb");
    if (! file) {
        free(buffer);
        fprintf(stderr, "cannot open '%s': %s\n", filename, strerror(errno));
        return 1;
    }

    /* read file -- file length must be < 32K */
    if (verbose)
        printf("reading input file...\n");
    filen = fread(buffer, 1, buflen, file);
    if (! filen) {
        fprintf(stderr, "read error\n");
    file_err:
        fclose(file);
        free(buffer);
        return 1;
    }
    if (filen > 32767l) {
        fprintf(stderr, "file is too large (must be < 32768)\n");
        goto file_err;
    }
    if (filen == buflen) { /* we have a buffer < 32768 and the file fits into it (and is most probably larger) */
        fprintf(stderr, "not enough memory\n");
        goto file_err;
    }
    if (verbose)
      printf("file size: %ld bytes\n", (long)filen);

    /* close input file */
    fclose(file);

    /* open cassette */
    if (verbose)
        printf("opening cassette...\n");
    iocb->buffer = C_dev;
    iocb->aux1 = 8;    /* open for output */
    iocb->aux2 = 128;  /* short breaks and no stop between data blocks */
    iocb->command = IOCB_OPEN;
    regs.x = iocb_num;
    regs.pc = 0xe456;   /* CIOV */

    _sys(&regs);
    if (regs.y != 1) {
        fprintf(stderr, "CIO call to open cassette returned %d\n", regs.y);
        free(buffer);
        return 1;
    }

    /* write file */
    if (verbose)
        printf("writing to cassette...\n");
    iocb->buffer = buffer;
    iocb->buflen = filen;
    iocb->command = IOCB_PUTCHR;
    regs.x = iocb_num;
    regs.pc = 0xe456;   /* CIOV */

    _sys(&regs);
    if (regs.y != 1) {
        fprintf(stderr, "CIO call to write file returned %d\n", regs.y);
        free(buffer);

        iocb->command = IOCB_CLOSE;
        regs.x = iocb_num;
        regs.pc = 0xe456;   /* CIOV */
        _sys(&regs);

        return 1;
    }

    /* free buffer */
    free(buffer);

    /* close cassette */
    iocb->command = IOCB_CLOSE;
    regs.x = iocb_num;
    regs.pc = 0xe456;   /* CIOV */
    _sys(&regs);

    if (regs.y != 1) {
        fprintf(stderr, "CIO call to close cassette returned %d\n", regs.y);
        return 1;
    }

    /* all is fine */
    printf("success\n");
    return 0;
}
