/*
  GEOS filesystem functions

  by Maciej 'YTM/Elysium' Witkowiak
*/

#ifndef _GFILE_H
#define _GFILE_H

#include <geos/gstruct.h>

struct filehandle *Get1stDirEntry(void);
struct filehandle *GetNxtDirEntry(void);

char __fastcall__ FindFTypes(char *buffer, char ftype, char fmaxnum, const char *classtxt);

char __fastcall__ GetFile(char flag, const char *fname,
                          const char *loadaddr, const char *datadname, const char *datafname);
char __fastcall__ FindFile(const char *fname);
char __fastcall__ ReadFile(struct tr_se *myTrSe, char *buffer, unsigned flength);
char __fastcall__ SaveFile(char skip, struct fileheader *myHeader);
char __fastcall__ FreeFile(struct tr_se myTable[]);
char __fastcall__ DeleteFile(const char *fname);
char __fastcall__ RenameFile(const char *source, const char *target);

char ReadByte(void);

char __fastcall__ FollowChain(struct tr_se *startTrSe, char *buffer);
char __fastcall__ GetFHdrInfo(struct filehandle *myFile);

char __fastcall__ OpenRecordFile(const char *fname);
char CloseRecordFile(void);
char NextRecord(void);
char PreviousRecord(void);
char __fastcall__ PointRecord(char);
char DeleteRecord(void);
char InsertRecord(void);
char AppendRecord(void);
char __fastcall__ ReadRecord(char *buffer, unsigned flength);
char __fastcall__ WriteRecord(const char *buffer, unsigned flength);
char UpdateRecordFile(void);

/* GEOS filetypes */
#define NOT_GEOS        0
#define BASIC           1
#define ASSEMBLY        2
#define DATA            3
#define SYSTEM          4
#define DESK_ACC        5
#define APPLICATION     6
#define APPL_DATA       7
#define FONT            8
#define PRINTER         9
#define INPUT_DEVICE    10
#define DISK_DEVICE     11
#define SYSTEM_BOOT     12
#define TEMPORARY       13
#define AUTO_EXEC       14
#define INPUT_128       15
#define NUMFILETYPES    16
/* supported structures */
#define SEQUENTIAL      0
#define VLIR            1
/* DOS filetypes */
#define DEL             0
#define SEQ             1
#define PRG             2
#define USR             3
#define REL             4
#define CBM             5

#endif
