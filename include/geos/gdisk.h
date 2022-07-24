/*
  GEOS functions from disk driver

  by Maciej 'YTM/Elysium' Witkowiak
*/

#ifndef _GDISK_H
#define _GDISK_H

#include <geos/gstruct.h>

char __fastcall__ ReadBuff(struct tr_se *myTrSe);
char __fastcall__ WriteBuff(struct tr_se *myTrSe);

char __fastcall__ GetBlock(struct tr_se *myTrSe, char *buffer);
char __fastcall__ PutBlock(struct tr_se *myTrSe, const char *buffer);
char __fastcall__ ReadBlock(struct tr_se *myTrSe, char *buffer);
char __fastcall__ WriteBlock(struct tr_se *myTrSe, const char *buffer);
char __fastcall__ VerWriteBlock(struct tr_se *myTrSe, const char *buffer);

unsigned CalcBlksFree(void);
char ChkDkGEOS(void);
char SetGEOSDisk(void);
char NewDisk(void);
char OpenDisk(void);

char __fastcall__ FindBAMBit(struct tr_se *myTrSe);
char __fastcall__ BlkAlloc(struct tr_se output[], unsigned length);
char __fastcall__ NxtBlkAlloc(struct tr_se *startTrSe,
                              struct tr_se output[], unsigned length);
char __fastcall__ FreeBlock(struct tr_se *myTrSe);
struct tr_se __fastcall__ SetNextFree(struct tr_se *myTrSe);
// above needs (unsigned) casts on both sides of '='

char GetDirHead(void);
char PutDirHead(void);
void __fastcall__ GetPtrCurDkNm(char *name);

void EnterTurbo(void);
void ExitTurbo(void);
void PurgeTurbo(void);

char __fastcall__ ChangeDiskDevice(char newdev);

/* disk header offsets i.e. index curDirHead with these */
#define OFF_TO_BAM      4
#define OFF_DISK_NAME   144
#define OFF_GS_DTYPE    189
#define OFF_OP_TR_SC    171
#define OFF_GS_ID       173
/* disk errors reported in _oserror */
#define ANY_FAULT       0xf0
#define G_EOF           0
#define NO_BLOCKS       1
#define INV_TRACK       2
#define INSUFF_SPACE    3
#define FULL_DIRECTORY  4
#define FILE_NOT_FOUND  5
#define BAD_BAM         6
#define UNOPENED_VLIR   7
#define INV_RECORD      8
#define OUT_OF_RECORDS  9
#define STRUCT_MISMAT   10
#define BFR_OVERFLOW    11
#define CANCEL_ERR      12
#define DEV_NOT_FOUND   13
#define INCOMPATIBLE    14
#define HDR_NOT_THERE   0x20
#define NO_SYNC         0x21
#define DBLK_NOT_THERE  0x22
#define DAT_CHKSUM_ERR  0x23
#define WR_VER_ERR      0x25
#define WR_PR_ON        0x26
#define HDR_CHKSUM_ERR  0x27
#define DSK_ID_MISMAT   0x29
#define BYTE_DEC_ERR    0x2e
#define DOS_MISMATCH    0x73

#endif
