#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <cbm.h>

int res = 0;
int tests = 0;

#define LFN     8

#define MAXREADSIZE    256

static char data[MAXREADSIZE];

void test1 (char * what, char * filename, int len1, int len2, int rlen1, int rlen2)
{
    int rlen;
    unsigned char err;

    printf ("1:%s (%d bytes)..." , what, rlen1);

    err = cbm_open (LFN, 8, 8, filename);
    if (err != 0) {
        printf ("\nError: could not open file.\n");
        goto test1exit;
    }

    rlen = cbm_read (LFN, data, rlen1);
    if (rlen == -1) {
        printf ("\ncbm_read error (%d)\n", _oserror);
        res++;
        goto test1exit;
    }
    if (rlen != len1) {
        printf ("\nError: 1st read returned %d instead of %d\n", rlen, len1);
        res++;
        goto test1exit;
    }

    printf (" OK\n");

    printf ("2:%s (%d bytes)..." , what, rlen2);

    rlen = cbm_read (LFN, data, rlen2);
    if (rlen == -1) {
        printf ("\ncbm_read error (%d)\n", _oserror);
        res++;
        goto test1exit;
    }
    if (rlen != len2) {
        printf ("\nError: 2nd read returned %d instead of %d\n", rlen, len2);
        res++;
        goto test1exit;
    }

    printf (" OK\n");

test1exit:
    cbm_close (LFN);
    tests++;
}

int main(void)
{
    test1 ("Read empty file",   "read-0", 0, 0, 0, 0);
    test1 ("Read empty file",   "read-0", 0, 0, 1, 1);
    test1 ("Read empty file",   "read-0", 0, 0, 8, 8);

    test1 ("Read 1 byte file",  "read-1", 0, 0, 0, 0);
    test1 ("Read 1 byte file",  "read-1", 1, 0, 1, 1);
    test1 ("Read 1 byte file",  "read-1", 1, 0, 8, 8);

    test1 ("Read 8 byte file",  "read-8", 0, 0, 0, 0);
    test1 ("Read 8 byte file",  "read-8", 1, 1, 1, 1);
    test1 ("Read 8 byte file",  "read-8", 4, 4, 4, 4);
    test1 ("Read 8 byte file",  "read-8", 8, 0, 8, 8);
    test1 ("Read 8 byte file",  "read-8", 8, 0,10, 1);

    printf ("%d/%d Tests failed.\n", res, tests);
    return res;
}
