#include <stdio.h>
#include <stdlib.h>
#include <serial.h>
#include <conio.h>
#include <dbg.h>


#if defined(__C64__)
#define DRIVERNAME      "c64-swlink.ser"
#elif defined(__C128__)
#define DRIVERNAME      "c128-swlink.ser"
#elif defined(__PLUS4__)
#define DRIVERNAME      "plus4-stdser.ser"
#elif defined(__CBM610__)
#define DRIVERNAME      "cbm610-std.ser"
#elif defined(__APPLE2ENH__)
#define DRIVERNAME      "a2e.ssc.ser"
#elif defined(__APPLE2__)
#define DRIVERNAME      "a2.ssc.ser"
#elif defined(__ATARIXL__)
#define DRIVERNAME      "atrxrdev.ser"
#elif defined(__ATARI__)
#define DRIVERNAME      "atrrdev.ser"
#else
#define DRIVERNAME      "unknown"
#error "Unknown target system"
#endif

extern unsigned int getsp(void);

static const struct ser_params Params = {
    SER_BAUD_9600,      /* Baudrate */
    SER_BITS_8,         /* Number of data bits */
    SER_STOP_1,         /* Number of stop bits */
    SER_PAR_NONE,       /* Parity setting */
    SER_HS_HW           /* Type of handshake to use */
};



static void CheckError (const char* Name, unsigned char Error)
{
    if (Error != SER_ERR_OK) {
        fprintf (stderr, "%s: %d\n", Name, Error);
    cgetc();
        exit (EXIT_FAILURE);
    }
}


int main (void)
{
    char Res;
    char C;
    printf("SP: $%04x\n", getsp());
    printf ("A\n");
    //cgetc();
    CheckError ("ser_load_driver", ser_load_driver (DRIVERNAME));
    printf ("B\n");
    //cgetc();
    printf("params at %p\n", &Params);
    CheckError ("ser_open", ser_open (&Params));
    printf ("C\n");
    //cgetc();
    printf("SP 2: $%04x\n", getsp());
    while (1) {
        if (kbhit ()) {
            printf("loop 1 SP: $%04x\n", getsp());
            C = cgetc ();
            if (C == '1') {
                break;
            } else {
                CheckError ("ser_put", ser_put (C));
                //printf ("%c", C);
            }
        }
        Res = ser_get (&C);
        if (Res != SER_ERR_NO_DATA) {
            printf("loop 2 SP: $%04x\n", getsp());
            CheckError ("ser_get", Res);
            printf ("%c", C);
        }
    }
    printf ("D\n");
    printf("after loop SP: $%04x\n", getsp());
    //cgetc();
    CheckError ("ser_unload", ser_unload ());
    printf ("E\n");

    cgetc();
    printf("final SP: $%04x\n", getsp());
    return EXIT_SUCCESS;
}
