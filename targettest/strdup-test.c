#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <_heap.h>

static unsigned char* V[256];

static void ShowInfo (void)
/* Show heap info */
{
    /* Count free blocks */
    unsigned Count = 0;
    unsigned** P = (unsigned**) _heapfirst;
    while (P) {
        ++Count;
        P = P[1];
    }
    printf ("%04X  %04X  %04X  %04X  %04X %u\n",
            _heaporg, _heapptr, _heapend, _heapfirst, _heaplast, Count);

    if (Count) {
        P = (unsigned**) _heapfirst;
        while (P) {
            printf ("%04X  %04X  %04X %04X(%u)\n",
                    (unsigned) P, P[2], P[1], P[0], P[0]);
            P = P[1];
        }
        getchar ();
    }
}



static const char* RandStr (void)
/* Create a random string */
{
    static char S [300];
    unsigned Len = (rand () & 0xFF) + (sizeof (S) - 0xFF - 1);
    unsigned I;
    char C;

    for (I = 0; I < Len; ++I) {
        do {
            C = rand() & 0xFF;
        } while (C == 0);
        S[I] = C;
    }
    S[Len] = '\0';

    return S;
}



static void FillArray (void)
/* Fill the string array */
{
    unsigned char I = 0;
    do {
        V[I] = strdup (RandStr ());
        ++I;
    } while (I != 0);
}



static void FreeArray (void)
/* Free all strings in the array */
{
    unsigned char I = 0;
    do {
        free (V[I]);
        ++I;
    } while (I != 0);
}



int main (void)
{
    unsigned long T;

    /* Show info at start */
    ShowInfo ();
#if !defined(__APPLE2__)
    /* Remember the time */
    T = clock ();
#endif
    /* Do the tests */
    FillArray ();
    ShowInfo ();
    FreeArray ();
    ShowInfo ();

#if !defined(__APPLE2__)
    /* Calculate the time and print it */
    T = clock () - T;
    printf ("Time needed: %lu ticks\n", T);
#endif

    /* Done */
    return EXIT_SUCCESS;
}



