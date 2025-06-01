#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

int signalcounter = 0;


void __fastcall__ sighandler (int sig)
{
    printf ("Got signal #%d\n", sig);
    signalcounter++;
}



int main (void)
{
    if (signal (SIGSEGV, sighandler) == SIG_ERR) {
        printf ("signal failure %d: %s\n", errno, strerror (errno));
        return EXIT_FAILURE;
    }
    printf ("About to raise SIGSEGV...\n");
    raise (SIGSEGV);
    printf ("Back from signal handler\n");
    printf ("About to raise SIGILL...\n");
    raise (SIGILL);
    printf ("Back from signal handler, signalcounter = %d\n", signalcounter);
    if (signalcounter != 1) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}


