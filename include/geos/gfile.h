/*
  GEOS filesystem functions

  by Maciej 'YTM/Elysium' Witkowiak
*/

#ifndef	_GFILE_H
#define _GFILE_H

#ifndef _GSTRUCT_H
#include <geos/gstruct.h>
#endif

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
#define	NOT_GEOS	0
#define	BASIC		1
#define	ASSEMBLY	2
#define	DATA		3
#define	SYSTEM		4
#define	DESK_ACC	5
#define	APPLICATION	6
#define	APPL_DATA	7
#define	FONT		8
#define	PRINTER		9
#define	INPUT_DEVICE	10
#define	DISK_DEVICE	11
#define	SYSTEM_BOOT	12
#define	TEMPORARY	13
#define	AUTO_EXEC	14
#define	INPUT_128	15
#define	NUMFILETYPES	16
/* supported structures */
#define	SEQUENTIAL	0
#define	VLIR		1
/* DOS filetypes */
#define	DEL		0
#define	SEQ		1
#define	PRG		2
#define	USR		3
#define	REL		4
#define	CBM		5
/* directory offsets */
/* offsets in dir entry i.e. index dirEntryBuf with these */
#define	FRST_FILE_ENTRY	2
#define	OFF_CFILE_TYPE	0
#define	OFF_DE_TR_SC	1
#define	OFF_FNAME	3
#define	OFF_GHDR_PTR	19
#define	OFF_GSTRUC_TYPE	21
#define	OFF_GFILE_TYPE	22
#define	OFF_YEAR	23
#define	OFF_SIZE	28
#define	OFF_NXT_FILE	32
/* offsets in file header i.e. index fileHeader with these */
#define	O_GHIC_WIDTH	2
#define	O_GHIC_HEIGHT	3
#define	O_GHIC_PIC	4
#define	O_GHCMDR_TYPE	68
#define	O_GHGEOS_TYPE	69
#define	O_GHSTR_TYPE	70
#define	O_GHST_ADDR	71
#define	O_GHEND_ADDR	73
#define	O_GHST_VEC	75
#define	O_GHFNAME	77
#define	O_128_FLAGS	96
#define	O_GH_AUTHOR	97
#define	O_GHP_DISK	97
#define	O_GHP_FNAME	117
#define	O_GHINFO_TXT	160

#endif
