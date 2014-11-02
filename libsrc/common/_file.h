/*
** _file.h
**
** (C) Copyright 1998, 2002 Ullrich von Bassewitz (uz@cc65.org)
**
*/



#ifndef __FILE_H
#define __FILE_H



#include <stdio.h>



/* Definition of struct _FILE */
struct _FILE {
    char            f_fd;
    char            f_flags;
    unsigned char   f_pushback;
};

/* File table. Beware: FOPEN_MAX is hardcoded in the ASM files! */
extern FILE _filetab[FOPEN_MAX];

/* Flags field */
#define _FCLOSED        0x00
#define _FOPEN          0x01
#define _FEOF           0x02
#define _FERROR         0x04
#define _FPUSHBACK      0x08



FILE* __fastcall__ _fopen (const char* name, const char* mode, FILE* f);
/* Open the specified file and fill the descriptor values into f */

FILE* _fdesc (void);
/* Find a free FILE descriptor */



/* End of _file.h */
#endif



