#include <stddef.h>
#include <stdlib.h>
#include <limits.h>
#include <conio.h>
#include <ctype.h>
#include <errno.h>
#include <dio.h>


#define MAX_CHUNKS 10 // Maximum acceptable number of chunks


static unsigned char ScreenX;
static unsigned char ScreenY;


static void ClearLine (void)
{
    cputc ('\r');
    cclear (ScreenX);
}


static driveid_t AskForDrive (char* Name)
{
    driveid_t Drive = 0;
    char      Char;

    cprintf ("\r\n%s Drive ID ?", Name);

    do {
        Char = cgetc ();
        if (isdigit (Char)) {
            cputc (Char);
            Drive = (driveid_t) (Drive * 10 + Char - '0');
        }
    } while (Char != CH_ENTER);

    return Drive;
}


static void AskForDisk (char* Name, driveid_t Drive)
{
    ClearLine ();
    cprintf ("\rInsert %s Disk into Drive %d !", Name, Drive);

    cgetc ();
}


static char* AllocBuffer (sectsize_t SectSize, sectnum_t SectCount, sectnum_t* ChunkCount)
{
    void*         Buffer = NULL;
    unsigned long BufferSize;
    unsigned int  Chunks = 1;

    do {
        *ChunkCount = (sectnum_t) ((SectCount + Chunks - 1) / Chunks);
        BufferSize = *ChunkCount * (unsigned long) SectSize;
        if (BufferSize < UINT_MAX) {
            Buffer = malloc ((size_t) BufferSize);
        }
    } while (Buffer == NULL && ++Chunks <= MAX_CHUNKS);

    return (char*) Buffer;
}


int main (void)
{
    driveid_t  SourceId;
    driveid_t  TargetId;
    dhandle_t  Source = NULL;
    dhandle_t  Target = NULL;
    sectsize_t SectSize;
    sectnum_t  SectCount;
    char*      Buffer;
    sectnum_t  Sector;
    sectnum_t  ChunkCount;
    sectnum_t  ChunkOffset = 0;

    clrscr ();
    screensize (&ScreenX, &ScreenY);

    cputs ("Floppy Disk Copy\r\n");
    chline (16);
    cputs ("\r\n");

    SourceId = AskForDrive ("Source");
    TargetId = AskForDrive ("Target");
    cputs ("\r\n\r\n");

    do {
        if (SourceId == TargetId || Source == NULL) {
            AskForDisk ("Source", SourceId);
        }

        if (Source == NULL) {
            Source = dio_open (SourceId);
            if (Source == NULL) {
                cprintf ("\r\n\r\nError %d on opening Drive %d\r\n", (int) _oserror, SourceId);
                return EXIT_FAILURE;
            }

            SectSize  = dio_query_sectsize (Source);
            SectCount = dio_query_sectcount (Source);

            Buffer = AllocBuffer (SectSize, SectCount, &ChunkCount);
            if (Buffer == NULL) {
                cputs ("\r\n\r\nError on allocating Buffer\r\n");
                return EXIT_FAILURE;
            }
        }

        ClearLine ();

        for (Sector = ChunkOffset; Sector < SectCount && (Sector - ChunkOffset) < ChunkCount; ++Sector) {
            cprintf ("\rReading Sector %d of %d", Sector + 1, SectCount);

            if (dio_read (Source, Sector, Buffer + (Sector - ChunkOffset) * SectSize) != 0) {
                cprintf ("\r\n\r\nError %d on reading Drive %d\r\n", (int) _oserror, SourceId);
                return EXIT_FAILURE;
            }
        }

        if (TargetId == SourceId || Target == NULL) {
            AskForDisk ("Target", TargetId);
        }

        if (Target == NULL) {
            Target = dio_open (TargetId);
            if (Target == NULL) {
                cprintf ("\r\n\r\nError %d on opening Drive %d\r\n", (int) _oserror, TargetId);
                return EXIT_FAILURE;
            }

            if (dio_query_sectsize (Target)  != SectSize ||
                dio_query_sectcount (Target) != SectCount) {
                cputs ("\r\n\r\nFormat mismatch between Drives\r\n");
                return EXIT_FAILURE;
            }
        }

        ClearLine ();

        for (Sector = ChunkOffset; Sector < SectCount && (Sector - ChunkOffset) < ChunkCount; ++Sector) {
            cprintf ("\rWriting Sector %d of %d", Sector + 1, SectCount);

            if (dio_write (Target, Sector, Buffer + (Sector - ChunkOffset) * SectSize) != 0) {
                cprintf ("\r\n\r\nError %d on opening Drive %d\r\n", (int) _oserror, TargetId);
                return EXIT_FAILURE;
            }
        }

        ChunkOffset += ChunkCount;

    } while (Sector < SectCount);

    ClearLine ();
    cprintf ("\rSuccessfully copied %d Sectors\r\n", SectCount);

    free (Buffer);
    dio_close (Source);
    dio_close (Target);

    return EXIT_SUCCESS;
}
