#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <_heap.h>



static unsigned char* V[256];



static char* Alloc (void)
/* Allocate a random sized chunk of memory */
{
    /* Determine the size */
    unsigned char Size = (((unsigned char)rand()) & 0x7F) + 1;

    /* Allocate memory */
    unsigned char* P = malloc (Size);

    /* Set the string to a defined value. We use the size, since this will
    ** also allow us to retrieve it later.
    */
    if (P) {
        memset (P, Size, Size);
    } else {
        printf ("Could not allocate %u bytes\n", Size);
        exit (EXIT_FAILURE);
    }
    return P;
}



static void Free (unsigned char* P)
/* Check a memory block and free it */
{
    unsigned char I;

    /* Get the size of the block */
    unsigned char Size = P[0];

    /* Scan the block */
    for (I = 1; I < Size; ++I) {
        if (P[I] != Size) {
            printf ("Scan failed - expected %02X, got %02X\n",
                    Size, P[I]);
            exit (EXIT_FAILURE);
        }
    }

    /* Free the block */
    free (P);
}



static void FillArray (void)
/* Fill the array with randomly allocated memory chunks */
{
    unsigned char I = 0;
    do {
        V[I] = Alloc ();
        ++I;
    } while (I != 0);
}



static void ShowInfo (void)
/* Show heap info */
{
    /* Count free blocks */
    unsigned Count = 0;
    register struct freeblock* P = _heapfirst;
    while (P) {
        ++Count;
        P = P->next;
    }
    printf ("%04X  %04X  %04X  %04X  %04X %u\n",
            _heaporg, _heapptr, _heapend, _heapfirst, _heaplast, Count);

    if (Count) {
        P = _heapfirst;
        while (P) {
            printf ("%04X  %04X  %04X %04X(%u)\n",
                    (unsigned) P, P[2], P[1], P[0], P[0]);
            P = P->next;
        }
        getchar ();
    }
}



static void Test1 (void)
{
    unsigned char I;
    FillArray ();
    for (I = 0; I < 0x80; ++I) {
        Free (V[0x7F-I]);
        Free (V[0x80+I]);
    }
    ShowInfo ();
}



static void Test2 (void)
{
    unsigned char I;
    FillArray ();
    I = 0;
    do {
        Free (V[I]);
        ++I;
    } while (I != 0);
    ShowInfo ();
}



static void Test3 (void)
{
    unsigned char I;
    FillArray ();
    I = 0;
    do {
        --I;
        Free (V[I]);
    } while (I != 0);
    ShowInfo ();
}



static void Test4 (void)
{
    unsigned char I;
    FillArray ();
    I = 0;
    do {
        Free (V[I]);
        I += 2;
    } while (I != 0);
    I = 1;
    do {
        Free (V[I]);
        I += 2;
    } while (I != 1);
    ShowInfo ();
}



static void Test5 (void)
{
    unsigned char I;
    FillArray ();
    I = 0;
    do {
        Free (V[I]);
        I += 2;
    } while (I != 0);
    do {
        V[I] = Alloc ();
        I += 2;
    } while (I != 0);
    I = 1;
    do {
        Free (V[I]);
        I += 2;
    } while (I != 1);
    do {
        V[I] = Alloc ();
        I += 2;
    } while (I != 1);
    I = 0;
    do {
        Free (V[I]);
        ++I;
    } while (I != 0);
    ShowInfo ();
}



static void Test6 (void)
{
    unsigned char I, J;
    FillArray ();
    I = J = 0;
    do {
        do {
            Free (V[I]);
            V[I] = Alloc ();
            ++I;
        } while (I != 0);
        ++J;
    } while (J < 5);
    do {
        Free (V[I]);
        ++I;
    } while (I != 0);
    ShowInfo ();
}



int main (void)
{
    unsigned long T;

    /* Show info at start */
    ShowInfo ();

    /* Remember the time */
    T = clock ();

    /* Do the tests */
    Test1 ();
    Test2 ();
    Test3 ();
    Test4 ();
    Test5 ();
    Test6 ();

    /* Calculate the time and print it */
    T = clock () - T;
    printf ("Time needed: %lu ticks\n", T);

    /* Done */
    return EXIT_SUCCESS;
}



