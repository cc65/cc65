/*
 * Ullrich von Bassewitz, 11.08.1998
 *
 * Hex conversion table. Must be in C since the compiler will convert
 * to the correct character set for the target platform.
 */



/* Data in this module is read-only, put it into the RODATA segment */
#pragma dataseg ("RODATA")

const unsigned char _hextab [16] = {
    '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};


