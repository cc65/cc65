/*****************************************************************************/
/* prof65                                                                    */
/* (C) 2023 Lauri Kasanen                                                    */
/*                                                                           */
/* This software is provided 'as-is', without any expressed or implied       */
/* warranty.  In no event will the authors be held liable for any damages    */
/* arising from the use of this software.                                    */
/*                                                                           */
/* Permission is granted to anyone to use this software for any purpose,     */
/* including commercial applications, and to alter it and redistribute it    */
/* freely, subject to the following restrictions:                            */
/*                                                                           */
/* 1. The origin of this software must not be misrepresented; you must not   */
/*    claim that you wrote the original software. If you use this software   */
/*    in a product, an acknowledgment in the product documentation would be  */
/*    appreciated but is not required.                                       */
/* 2. Altered source versions must be plainly marked as such, and must not   */
/*    be misrepresented as being the original software.                      */
/* 3. This notice may not be removed or altered from any source              */
/*    distribution.                                                          */
/*                                                                           */
/*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static unsigned samples[64 * 1024];

struct label_t {
    char name[32];
    unsigned short addr;
    unsigned count;
};

#define MAX_LABELS 1024
static struct label_t labels[MAX_LABELS];
static unsigned numlabels;

static int byaddr(const void *ap, const void *bp)
{
    const struct label_t *a = (struct label_t *) ap;
    const struct label_t *b = (struct label_t *) bp;

    if (a->addr < b->addr) {
        return -1;
    }
    if (a->addr > b->addr) {
        return 1;
    }
    return 0;
}

static int bycount(const void *ap, const void *bp)
{
    const struct label_t *a = (struct label_t *) ap;
    const struct label_t *b = (struct label_t *) bp;

    if (a->count > b->count) {
        return -1;
    }
    if (a->count < b->count) {
        return 1;
    }
    return 0;
}

static struct label_t *findlabel(const unsigned short addr)
{
    unsigned i;
    for (i = 1; i < numlabels; i++) {
        if (labels[i - 1].addr <= addr && labels[i].addr > addr) {
            return &labels[i - 1];
        } else if (i == numlabels - 1 && labels[i].addr <= addr) {
            return &labels[i];
        }
    }

    return NULL;
}

static void usage(const char *argv0)
{
    printf ("Usage: %s [args] profile label\n\n"
            "-v\tVerbose\n", argv0);
    exit (0);
}

int main(int argc, char **argv)
{
    unsigned char verbose = 0;
    char buf[1024];
    const char *profile = NULL, *label = NULL;
    int opt;
    unsigned i, total;
    FILE *f;

    while ((opt = getopt (argc, argv, "hv")) != -1) {
        switch (opt) {
            case 'h':
            default:
                usage (argv[0]);
            break;
            case 'v':
                verbose = 1;
            break;
        }
    }

    if (optind + 2 > argc) {
        usage (argv[0]);
    }

    profile = argv[optind];
    label = argv[optind + 1];

    /* samples */
    f = fopen (profile, "rb");
    if (!f) {
        printf("Can't open %s\n", profile);
        return 1;
    }

    if (fread (samples, 1, sizeof(samples), f) != sizeof(samples)) {
        printf("Invalid profile\n");
        return 1;
    }

    fclose (f);

    /* labels */
    f = fopen (label, "r");
    if (!f) {
        printf("Can't open %s\n", label);
        return 1;
    }

    numlabels = 0;
    while (fgets (buf, 1024, f)) {
        unsigned i, len;
        struct label_t *cur;
        for (i = 0; buf[i]; i++) {
            if (buf[i] == '\n') {
                buf[i] = '\0';
                break;
            }
        }

        if (strstr (buf, ".__")) { /* skip metalabels */
            continue;
        }

        cur = &labels[numlabels];
        cur->count = 0;
        cur->addr = strtoul(buf + 3, NULL, 16);

        len = strlen(buf + 11);
        if (len > 31) {
            len = 31;
        }
        memcpy(cur->name, buf + 11, len);
        cur->name[len] = '\0';

        numlabels++;
        if (numlabels >= MAX_LABELS) {
            printf ("Too many labels\n");
            break;
        }
    }

    fclose (f);

    /* prep */
    qsort (labels, numlabels, sizeof(struct label_t), byaddr);

    total = 0;
    for (i = 0; i < 64 * 1024; i++) {
        struct label_t *cur;
        if (!samples[i]) {
            continue;
        }

        cur = findlabel(i);
        cur->count += samples[i];
        total += samples[i];
    }

    qsort (labels, numlabels, sizeof(struct label_t), bycount);

    /* output */
    printf ("CPU cycles total: %u\n\n", total);
    for (i = 0; i < numlabels; i++) {
        if ((i >= 10 || labels[i].count < total / 100) && !verbose) {
            break;
        }
        if (!labels[i].count) {
            break;
        }
        printf ("%.1f%%\t%8u\t%s\n", labels[i].count * 100.0f / total,
                labels[i].count,
                labels[i].name);
    }

    return 0;
}
