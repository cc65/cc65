/*
  GEOS system functions

  ported to small C on 27.10.1999
  by Maciej 'YTM/Elysium' Witkowiak
*/

#ifndef	_GSYS_H
#define _GSYS_H

void __fastcall__ FirstInit(void);
void __fastcall__ InitForIO(void);
void __fastcall__ DoneWithIO(void);
void __fastcall__ MainLoop(void);
void __fastcall__ EnterDeskTop(void);
void __fastcall__ ToBASIC(void);
void __fastcall__ Panic(void);

void __fastcall__ CallRoutine(void *myRoutine);

unsigned __fastcall__ GetSerialNumber(void);
char __fastcall__ GetRandom(void);

void __fastcall__ SetDevice(char newdev);

char __fastcall__ get_ostype(void);
/* possible return values of get_ostype, machine and version flags will
   be combined */
#define GEOS64		0x00
#define GEOS128		0x80
#define GEOS_V10	0x10
#define GEOS_V12	0x12	/* ??? not sure */
#define GEOS_V20	0x20

char __fastcall__ get_tv(void);
/* possible return values of get_tv, these flags will be combined
   note that columns state can be changed during runtime */
#define COLUMNS40	0x00
#define COLUMNS80	0x01
#define TV_PAL		0x00
#define TV_NTSC		0x80

#endif
