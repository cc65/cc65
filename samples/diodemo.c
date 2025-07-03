/*****************************************************************************/
/*                                                                           */
/*                                 diodemo.c                                 */
/*                                                                           */
/*                       Direct Disk I/O Demo Program                        */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) Copyright 2005, Oliver Schmidt, <ol.sc@web.de>                        */
/*                                                                           */
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



#define DYN_BOX_DRAW
#include <stddef.h>
#include <stdlib.h>
#include <limits.h>
#include <conio.h>
#include <ctype.h>
#include <errno.h>
#include <cc65.h>
#include <dio.h>


#define MAX_CHUNKS 10 /* Maximum acceptable number of chunks */


static unsigned char ScreenX;
static unsigned char ScreenY;


static void ClearLine (void)
/* Clear the screen line the cursor is on */
{
    cputc ('\r');
    cclear (ScreenX);
}


static unsigned char AskForDrive (const char* Name)
/* Ask for a drive id and return it */
{
    unsigned char Drive = 0;
    char          Char;

    cprintf ("\r\n%s Drive ID ? ", Name);

    cursor (1);
    do {
        Char = cgetc ();
        if (isdigit (Char)) {
            cputc (Char);
            Drive = Drive * 10 + Char - '0';
        }
    } while (Char != CH_ENTER);
    cursor (0);

    return Drive;
}


static void AskForDisk (const char* Name, unsigned char Drive)
/* Ask the user to insert a specific disk */
{
    ClearLine ();
    cprintf ("\rInsert %s Disk into Drive %d !", Name, Drive);

    cgetc ();
}


static char* AllocBuffer (unsigned int SectSize, unsigned int SectCount, unsigned int* ChunkCount)
/* Allocate a copy buffer on the heap and return a pointer to it */
{
    char*         Buffer = NULL;
    unsigned long BufferSize;
    unsigned int  Chunks = 1;

    /* Increase number of chunks resp. decrease size */
    /* of one chunk until buffer allocation succeeds */
    do {
        *ChunkCount = (unsigned int) ((SectCount + Chunks - 1) / Chunks);
        BufferSize = *ChunkCount * (unsigned long) SectSize;
        if (BufferSize < UINT_MAX) {
            Buffer = malloc ((size_t) BufferSize);
        }
    } while (Buffer == NULL && ++Chunks <= MAX_CHUNKS);

    return Buffer;
}


int main (int argc, const char* argv[])
{
    unsigned char SourceId;
    unsigned char TargetId;
    dhandle_t     Source = NULL;
    dhandle_t     Target = NULL;
    unsigned int  SectSize;
    unsigned int  SectCount;
    char*         Buffer;
    unsigned int  Sector;
    unsigned int  ChunkCount;
    unsigned int  ChunkOffset = 0;

    clrscr ();
    screensize (&ScreenX, &ScreenY);

    /* Allow user to read exit messages */
    if (doesclrscrafterexit ()) {
        atexit ((void (*)) cgetc);
    }

    cputs ("Floppy Disk Copy\r\n");
    chline (16);
    cputs ("\r\n");

    /* Get source and target drive id (which may very well be identical) */
    switch (argc) {
      case 1:
        SourceId = AskForDrive ("Source");
        TargetId = AskForDrive ("Target");
        cputs ("\r\n");
        break;

      case 2:
        SourceId = TargetId = atoi (argv[1]);
        break;

      case 3:
        SourceId = atoi (argv[1]);
        TargetId = atoi (argv[2]);
        break;

      default:
        cprintf ("\r\nToo many arguments\r\n");
        return EXIT_FAILURE;
    }

    cputs ("\r\n");

    do {
        /* Check for single drive copy or inital iteration */
        if (SourceId == TargetId || Source == NULL) {
            AskForDisk ("Source", SourceId);
        }

        /* Check for initial iteration */
        if (Source == NULL) {

            /* Open source drive */
            Source = dio_open (SourceId);
            if (Source == NULL) {
                cprintf ("\r\n\nError %d on opening Drive %d\r\n", (int) _oserror, SourceId);
                return EXIT_FAILURE;
            }

            SectSize  = dio_query_sectsize (Source);
            SectCount = dio_query_sectcount (Source);

            /* Allocate buffer */
            Buffer = AllocBuffer (SectSize, SectCount, &ChunkCount);
            if (Buffer == NULL) {
                cputs ("\r\n\nError on allocating Buffer\r\n");
                return EXIT_FAILURE;
            }
        }

        ClearLine ();

        /* Read one chunk of sectors into buffer */
        for (Sector = ChunkOffset; Sector < SectCount && (Sector - ChunkOffset) < ChunkCount; ++Sector) {
            cprintf ("\rReading Sector %d of %d", Sector + 1, SectCount);

            /* Read one sector */
            if (dio_read (Source, Sector, Buffer + (Sector - ChunkOffset) * SectSize) != 0) {
                cprintf ("\r\n\nError %d on reading from Drive %d\r\n", (int) _oserror, SourceId);
                return EXIT_FAILURE;
            }
        }

        /* Check for single drive copy or inital iteration */
        if (TargetId == SourceId || Target == NULL) {
            AskForDisk ("Target", TargetId);
        }

        /* Open target drive on initial iteration */
        if (Target == NULL) {
            Target = dio_open (TargetId);
            if (Target == NULL) {
                cprintf ("\r\n\nError %d on opening Drive %d\r\n", (int) _oserror, TargetId);
                return EXIT_FAILURE;
            }

            /* Check for compatible drives */
            if (dio_query_sectsize (Target)  != SectSize ||
                dio_query_sectcount (Target) != SectCount) {
                cputs ("\r\n\nFormat mismatch between Drives\r\n");
                return EXIT_FAILURE;
            }
        }

        ClearLine ();

        /* Write one chunk of sectors from buffer */
        for (Sector = ChunkOffset; Sector < SectCount && (Sector - ChunkOffset) < ChunkCount; ++Sector) {
            cprintf ("\rWriting Sector %d of %d", Sector + 1, SectCount);

            /* Write one sector */
            if (dio_write (Target, Sector, Buffer + (Sector - ChunkOffset) * SectSize) != 0) {
                cprintf ("\r\n\nError %d on writing to Drive %d\r\n", (int) _oserror, TargetId);
                return EXIT_FAILURE;
            }
        }

        /* Advance to next chunk */
        ChunkOffset += ChunkCount;

    } while (Sector < SectCount);

    ClearLine ();
    cprintf ("\rSuccessfully copied %d Sectors\r\n", SectCount);

    free (Buffer);
    dio_close (Source);
    dio_close (Target);

    return EXIT_SUCCESS;
}
