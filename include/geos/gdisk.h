/*
  GEOS functions from disk driver

  ported to small C on 21.12.1999
  by Maciej 'YTM/Elysium' Witkowiak
*/

#ifndef	_GDISK_H
#define _GDISK_H

#ifndef _GSTRUCT_H
#include <geos/gstruct.h>
#endif

char __fastcall__ ReadBuff(struct tr_se *myTrSe);
char __fastcall__ WriteBuff(struct tr_se *myTrSe);

char __fastcall__ GetBlock(struct tr_se *myTrSe, char *buffer);
char __fastcall__ PutBlock(struct tr_se *myTrSe, const char *buffer);
char __fastcall__ ReadBlock(struct tr_se *myTrSe, char *buffer);
char __fastcall__ WriteBlock(struct tr_se *myTrSe, const char *buffer);
char __fastcall__ VerWriteBlock(struct tr_se *myTrSe, const char *buffer);

int __fastcall__ CalcBlksFree(void);
char __fastcall__ ChkDkGEOS(void);
char __fastcall__ SetGEOSDisk(void);
char __fastcall__ NewDisk(void);
char __fastcall__ OpenDisk(void);

char __fastcall__ FindBAMBit(struct tr_se *myTrSe);
char __fastcall__ BlkAlloc(struct tr_se output[], int length);
char __fastcall__ NxtBlkAlloc(struct tr_se *startTrSe,
			      struct tr_se output[], int length);
char __fastcall__ FreeBlock(struct tr_se *myTrSe);
struct tr_se __fastcall__ SetNextFree(struct tr_se *myTrSe);
// above needs (int) casts on both sides of '='

char __fastcall__ GetDirHead(void);
char __fastcall__ PutDirHead(void);
void __fastcall__ GetPtrCurDkNm(char *name);

void __fastcall__ EnterTurbo(void);
void __fastcall__ ExitTurbo(void);
void __fastcall__ PurgeTurbo(void);

char __fastcall__ ChangeDiskDevice(char newdev);

/* disk header offsets	*/
#define	OFF_TO_BAM	4
#define	OFF_DISK_NAME	144
#define	OFF_GS_DTYPE	189
#define	OFF_OP_TR_SC	171
#define	OFF_GS_ID	173
/* disk errors */
#define	ANY_FAULT	0xf0
#define G_EOF		0
#define	NO_BLOCKS	1
#define	INV_TRACK	2
#define	INSUFF_SPACE	3
#define	FULL_DIRECTORY	4
#define	FILE_NOT_FOUND	5
#define	BAD_BAM		6
#define	UNOPENED_VLIR	7
#define	INV_RECORD	8
#define	OUT_OF_RECORDS	9
#define	STRUCT_MISMAT	10
#define	BFR_OVERFLOW	11
#define	CANCEL_ERR	12
#define	DEV_NOT_FOUND	13
#define	INCOMPATIBLE	14
#define	HDR_NOT_THERE	0x20
#define	NO_SYNC		0x21
#define	DBLK_NOT_THERE	0x22
#define	DAT_CHKSUM_ERR	0x23
#define	WR_VER_ERR	0x25
#define	WR_PR_ON	0x26
#define	HDR_CHKSUM_ERR	0x27
#define	DSK_ID_MISMAT	0x29
#define	BYTE_DEC_ERR	0x2e
#define	DOS_MISMATCH	0x73

#endif
