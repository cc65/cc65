/*
 * errormsg.c
 *
 * Ullrich von Bassewitz, 17.05.2000
 *
 * Must be a C function, since we have otherwise problems with the different
 * character sets.
 */



const char* const _sys_errlist[] = {
    "Unknown error",                /*  0 */
    "No such file or directory",    /*  1 */
    "Out of memory",                /*  2 */
    "Permission denied",            /*  3 */
    "No such device",               /*  4 */
    "Too many open files",          /*  5 */
    "Device or resource busy",      /*  6 */
    "Invalid argument",             /*  7 */
    "No space left on device",      /*  8 */
    "File exists",                  /*  9 */
    "Try again",                    /* 10 */
    "I/O error",                    /* 11 */
    "Interrupted system call",      /* 12 */
    "Function not implemented",     /* 13 */
    "Illegal seek",                 /* 14 */
    "Range error",                  /* 15 */
};


