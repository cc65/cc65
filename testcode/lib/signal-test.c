#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <signal.h>


void __fastcall__ sighandler (int sig)
{
    printf ("Got signal #%d\n", sig);
}



int main (void)
{
    if (signal (SIGSEGV, sighandler) == SIG_ERR) {
        printf ("signal failure %d: %s\n", errno, strerror (errno));
        return 1;
    }
    printf ("About to raise SIGSEGV...\n");
    raise (SIGSEGV);
    printf ("Back from signal handler\n");
    printf ("About to raise SIGILL...\n");
    raise (SIGILL);
    printf ("Back from signal handler\n");
    return 0;
}


